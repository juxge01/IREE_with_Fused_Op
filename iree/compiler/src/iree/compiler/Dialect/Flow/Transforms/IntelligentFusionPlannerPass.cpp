// IntelligentFusionPlannerPass.cpp
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Flow/IR/FlowDialect.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "iree-flow-intelligent-fusion-planner"

#include <cmath>
#include <algorithm>

namespace mlir::iree_compiler::IREE::Flow {

static inline bool isArithMaxMinFOp(Operation *op) {
  return isa<arith::MaximumFOp, arith::MinimumFOp>(op);
}

static long long countFlopsInRegion(Region &region) {
  int64_t flops = 0;
  if (region.empty()) return flops;
  region.walk([&](Operation *op) {
    if (isa<arith::AddFOp, arith::SubFOp, arith::MulFOp, arith::DivFOp>(op) ||
        isArithMaxMinFOp(op)) {
      flops++;
    } else if (isa<math::FmaOp>(op)) {
      flops += 2;
    } else if (isa<math::ExpOp, math::Exp2Op, math::TanhOp, math::SqrtOp,
                math::RsqrtOp, math::LogOp, math::Log2Op, math::Log10Op>(op)) {
      flops += 4;
    }
  });
  return flops;
}

// TableGen으로 생성되는 베이스/옵션 심볼을 이 파일에서 활성화
#define GEN_PASS_DEF_INTELLIGENTFUSIONPLANNERPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {
// File-scope command-line options. llvm::cl::opt is non-copyable and is intended
// to live at static/file scope rather than as members on pass instances. Keeping
// them file-scoped avoids making the pass non-copyable (the pass base expects
// the derived pass to be copy-constructible for cloning).
static llvm::cl::opt<double> pCpu("iree-intel-planner-pcpu",
                                 llvm::cl::desc("CPU FLOP/s"),
                                 llvm::cl::init(10e9));
static llvm::cl::opt<double> bwCpu("iree-intel-planner-bwcpu",
                                  llvm::cl::desc("CPU memory bandwidth bytes/s"),
                                  llvm::cl::init(20e9));
static llvm::cl::opt<double> pAcc("iree-intel-planner-pacc",
                                 llvm::cl::desc("Accelerator FLOP/s"),
                                 llvm::cl::init(100e9));
static llvm::cl::opt<double> bwAcc("iree-intel-planner-bwacc",
                                  llvm::cl::desc("Accelerator memory bandwidth bytes/s"),
                                  llvm::cl::init(100e9));
static llvm::cl::opt<double> bwH2D("iree-intel-planner-bwh2d",
                                  llvm::cl::desc("Host->Device bandwidth bytes/s"),
                                  llvm::cl::init(10e9));
static llvm::cl::opt<double> bwD2H("iree-intel-planner-bwd2h",
                                  llvm::cl::desc("Device->Host bandwidth bytes/s"),
                                  llvm::cl::init(10e9));
static llvm::cl::opt<double> launchOverhead("iree-intel-planner-launch",
                                            llvm::cl::desc("Accelerator launch overhead seconds"),
                                            llvm::cl::init(1e-5));

// Target name for offloading (string). Default to the runtime driver id
// "accel" so the planner's `offload.target` attribute maps to the accel
// executable loader/driver. Users can override via command-line if needed.
static llvm::cl::opt<std::string> accelTargetName(
  "iree-intel-planner-accel-target",
  llvm::cl::desc("Offload target name to set on selected ops/dispatches"),
  llvm::cl::init(std::string("accel")));

struct IntelligentFusionPlannerPass
    : public impl::IntelligentFusionPlannerPassBase<IntelligentFusionPlannerPass> {
  using impl::IntelligentFusionPlannerPassBase<
      IntelligentFusionPlannerPass>::IntelligentFusionPlannerPassBase;

  // 편의 생성자(파이프라인 코드에서 ratio 직접 주입 가능)
  explicit IntelligentFusionPlannerPass(double ratio) { this->topKRatio = ratio; }

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<IREE::Flow::FlowDialect, mlir::arith::ArithDialect,
                    mlir::linalg::LinalgDialect, mlir::tensor::TensorDialect,
                    mlir::math::MathDialect, mlir::func::FuncDialect>();
  }
  
  void runOnOperation() override {
    ModuleOp module = getOperation();
    OpBuilder builder(&getContext());

    // [수정] 모든 연산을 FLOPs와 함께 저장합니다: {FLOPs, Operation*}
    SmallVector<std::pair<long long, Operation *>> allOpsWithFlops;
    // 추가: 연산별 바이트 및 기대 이득을 기록하기 위해 임시 컨테이너
    SmallVector<std::tuple<long long, long long, double, Operation *>> allOpsWithMetrics;

    // 1) 함수별 FLOPs 계산(이미 dispatch 내부는 제외)
    for (auto funcOp : module.getOps<FunctionOpInterface>()) {
      // 이미 executable 내부면 스킵
      if (funcOp->getParentOfType<IREE::Flow::ExecutableOp>()) continue;
      // 외부 선언/바디 없음 스킵(안전)
      if (!funcOp.isDeclaration() && funcOp->getBlock() == nullptr) continue;

      funcOp.walk([&](Operation *op) {
        if (op->getParentOfType<IREE::Flow::DispatchRegionOp>()) return;

        long long currentOpFlops = 0;
        long long currentOpBytes = 0;
        double currentOpBenefit = 0.0;
        // element byte 계산 헬퍼
        auto getElementBytes = [](Type t) -> long long {
          if (auto ft = dyn_cast<FloatType>(t)) return ft.getWidth() / 8;
          // 기본값: 4바이트
          return 4;
        };
        // 동적 차원은 1로 대체(기존 FLOPs 로직과 동일한 보수적 근사)
        auto getDimOrOne = [](int64_t d) { return d == ShapedType::kDynamic ? 1 : d; };
        TypeSwitch<Operation *>(op)
            .Case<linalg::MatmulOp>([&](auto m) {
              auto lhsType =
                  dyn_cast<RankedTensorType>(m.getDpsInputOperand(0)->get().getType());
              auto rhsType =
                  dyn_cast<RankedTensorType>(m.getDpsInputOperand(1)->get().getType());
              if (!lhsType || !rhsType || !lhsType.hasStaticShape() ||
                  !rhsType.hasStaticShape() || lhsType.getRank() != 2 ||
                  rhsType.getRank() != 2)
                return;
              auto lhsShape = lhsType.getShape();
              auto rhsShape = rhsType.getShape();
              long long M = lhsShape[0], K = lhsShape[1], N = rhsShape[1];
              currentOpFlops = 2LL * M * K * N;
              long long elemBytes = getElementBytes(lhsType.getElementType());
              currentOpBytes = elemBytes * (M * K + K * N + M * N);
            })
            .Case<linalg::BatchMatmulOp>([&](auto m) {
              auto lhsType =
                  dyn_cast<RankedTensorType>(m.getDpsInputOperand(0)->get().getType());
              auto rhsType =
                  dyn_cast<RankedTensorType>(m.getDpsInputOperand(1)->get().getType());
              if (!lhsType || !rhsType || !lhsType.hasStaticShape() ||
                  !rhsType.hasStaticShape() || lhsType.getRank() != 3 ||
                  rhsType.getRank() != 3)
                return;
              auto lhsShape = lhsType.getShape();
              auto rhsShape = rhsType.getShape();
              long long B = lhsShape[0], M = lhsShape[1], K = lhsShape[2], N = rhsShape[2];
              currentOpFlops = B * 2LL * M * K * N;
              long long elemBytes = getElementBytes(lhsType.getElementType());
              currentOpBytes = elemBytes * (B * M * K + B * K * N + B * M * N);
            })
            .Case<linalg::Conv2DNhwcHwcfOp>([&](auto conv) {
              auto inTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInputOperand(0)->get().getType());
              auto fTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInputOperand(1)->get().getType());
              auto outTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInitOperand(0)->get().getType());
              if (!inTy || !fTy || !outTy) return;
              long long N = getDimOrOne(outTy.getDimSize(0));
              long long H = getDimOrOne(outTy.getDimSize(1));
              long long W = getDimOrOne(outTy.getDimSize(2));
              long long Co = getDimOrOne(outTy.getDimSize(3));
              long long Kh = getDimOrOne(fTy.getDimSize(0));
              long long Kw = getDimOrOne(fTy.getDimSize(1));
              long long Ci = getDimOrOne(fTy.getDimSize(2));
              currentOpFlops = 2LL * N * H * W * Co * Kh * Kw * Ci;
              long long elemBytes = getElementBytes(outTy.getElementType());
              // input + filter + output
              long long inElems = getDimOrOne(inTy.getDimSize(0)) * getDimOrOne(inTy.getDimSize(1)) * getDimOrOne(inTy.getDimSize(2)) * getDimOrOne(inTy.getDimSize(3));
              long long fElems = Kh * Kw * Ci * getDimOrOne(fTy.getDimSize(3));
              long long outElems = N * H * W * Co;
              currentOpBytes = elemBytes * (inElems + fElems + outElems);
            })
            .Case<linalg::Conv2DNchwFchwOp>([&](auto conv) {
              auto inTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInputOperand(0)->get().getType());
              auto fTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInputOperand(1)->get().getType());
              auto outTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInitOperand(0)->get().getType());
              if (!inTy || !fTy || !outTy) return;
              long long N = getDimOrOne(outTy.getDimSize(0));
              long long Co = getDimOrOne(outTy.getDimSize(1));
              long long H = getDimOrOne(outTy.getDimSize(2));
              long long W = getDimOrOne(outTy.getDimSize(3));
              long long Ci = getDimOrOne(fTy.getDimSize(1));
              long long Kh = getDimOrOne(fTy.getDimSize(2));
              long long Kw = getDimOrOne(fTy.getDimSize(3));
              currentOpFlops = 2LL * N * H * W * Co * Kh * Kw * Ci;
              long long elemBytes = getElementBytes(outTy.getElementType());
              long long inElems = getDimOrOne(inTy.getDimSize(0)) * getDimOrOne(inTy.getDimSize(1)) * getDimOrOne(inTy.getDimSize(2)) * getDimOrOne(inTy.getDimSize(3));
              long long fElems = getDimOrOne(fTy.getDimSize(0)) * getDimOrOne(fTy.getDimSize(1)) * getDimOrOne(fTy.getDimSize(2)) * getDimOrOne(fTy.getDimSize(3));
              long long outElems = N * Co * H * W;
              currentOpBytes = elemBytes * (inElems + fElems + outElems);
            })
            .Case<linalg::DepthwiseConv2DNhwcHwcOp>([&](auto conv) {
              auto fTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInputOperand(1)->get().getType());
              auto outTy = dyn_cast<RankedTensorType>(
                  conv.getDpsInitOperand(0)->get().getType());
              if (!fTy || !outTy) return;
              long long N = getDimOrOne(outTy.getDimSize(0));
              long long H = getDimOrOne(outTy.getDimSize(1));
              long long W = getDimOrOne(outTy.getDimSize(2));
              long long C = getDimOrOne(outTy.getDimSize(3));
              long long Kh = getDimOrOne(fTy.getDimSize(0));
              long long Kw = getDimOrOne(fTy.getDimSize(1));
              currentOpFlops = 2LL * N * H * W * C * Kh * Kw;
              long long elemBytes = getElementBytes(outTy.getElementType());
              long long inElems = N * H * W * C; // approximate
              long long fElems = Kh * Kw * getDimOrOne(fTy.getDimSize(2));
              long long outElems = inElems;
              currentOpBytes = elemBytes * (inElems + fElems + outElems);
            })
            .Case<linalg::GenericOp>([&](linalg::GenericOp g) {
              if (!g.hasPureTensorSemantics() || g.getNumResults() == 0) return;
              SmallVector<int64_t, 4> loopRanges = g.getStaticLoopRanges();
              long long iters = 1;
              for (int64_t r : loopRanges) iters *= ShapedType::isDynamic(r) ? 1 : r;
              if (!iters) return;
              long long perIter = 0;
              if (!g.getRegion().empty()) {
                g.getRegion().walk([&](Operation *inner) {
                  if (isa<arith::AddFOp, arith::SubFOp, arith::MulFOp,
                          arith::CmpFOp, arith::SelectOp, math::AbsFOp>(inner)) {
                    perIter++;
                  } else if (isa<arith::DivFOp, math::ExpOp, math::LogOp, math::SqrtOp,
                                 math::RsqrtOp, math::TanhOp, math::PowFOp>(inner)) {
                    perIter += 4;
                  }
                });
              }
              if (perIter) currentOpFlops = iters * perIter;
              // GenericOp: 요소 개수는 루프 범위의 곱, 바이트는 결과/입력들의 요소 합으로 근사
              long long elemBytes = 0;
              // try to infer element size from first result or first input
              if (g.getNumResults() > 0) {
                if (auto outTy = dyn_cast<RankedTensorType>(g.getResult(0).getType()))
                  elemBytes = getElementBytes(outTy.getElementType());
              }
              if (!elemBytes && !g.getInputs().empty()) {
                if (auto inTy = dyn_cast<RankedTensorType>(g.getInputs()[0].getType()))
                  elemBytes = getElementBytes(inTy.getElementType());
              }
              if (elemBytes) {
                // approximate bytes as iters * per-tuple-bytes (inputs+outputs)
                long long perTupleElems = 0;
                for (Value v : g.getInputs())
                  if (auto t = dyn_cast<RankedTensorType>(v.getType()))
                    perTupleElems += 1; // can't easily know per-operand elems -> conservative 1
                for (Value v : g.getResults())
                  if (auto t = dyn_cast<RankedTensorType>(v.getType()))
                    perTupleElems += 1;
                currentOpBytes = elemBytes * iters * (perTupleElems);
              }
            })
            .Case<linalg::AddOp, linalg::MulOp, linalg::SubOp>([&](Operation *linOp) {
              if (linOp->getNumResults() == 0) return;
              auto outTy = dyn_cast<RankedTensorType>(linOp->getResult(0).getType());
              if (!outTy || !outTy.hasStaticShape() ||
                  !isa<FloatType>(outTy.getElementType()))
                return;
              long long elems = 1;
              for (int64_t d : outTy.getShape()) elems *= d;
              currentOpFlops = elems;
              long long elemBytes = getElementBytes(outTy.getElementType());
              // input(s) + output
              long long inElems = 0;
              for (Value v : linOp->getOperands()) if (auto t = dyn_cast<RankedTensorType>(v.getType())) {
                if (t.hasStaticShape()) {
                  long long e = 1; for (int64_t di : t.getShape()) e *= di; inElems += e;
                }
              }
              currentOpBytes = elemBytes * (inElems + elems);
            })
            .Case<math::ExpOp, math::Exp2Op, math::TanhOp, math::SqrtOp,
                  math::RsqrtOp, math::LogOp, math::Log2Op, math::Log10Op>(
                [&](Operation *m) {
                  if (m->getNumResults() == 0) return;
                  auto outTy = dyn_cast<RankedTensorType>(m->getResult(0).getType());
                  if (!outTy || !outTy.hasStaticShape() ||
                      !isa<FloatType>(outTy.getElementType()))
                    return;
                  long long elems = 1;
                  for (int64_t d : outTy.getShape()) elems *= d;
                  currentOpFlops = elems * 4;
                  long long elemBytes = getElementBytes(outTy.getElementType());
                  currentOpBytes = elemBytes * elems * 2; // approx: input + output
                })
            .Case<linalg::DepthwiseConv2DNchwChwOp>([&](auto dw) {
              auto inTy  = dyn_cast<RankedTensorType>(dw.getInputs()[0].getType());
              auto filTy = dyn_cast<RankedTensorType>(dw.getInputs()[1].getType());
              auto outTy = dyn_cast<RankedTensorType>(dw.getOutputs()[0].getType());
              if (!inTy || !filTy || !outTy || !outTy.hasStaticShape() ||
                  !isa<FloatType>(outTy.getElementType()))
                return;
              long long outElems = 1;
              for (int64_t d : outTy.getShape()) outElems *= d;
              long long kElems = filTy.getShape()[1] * filTy.getShape()[2];
              currentOpFlops = 2LL * outElems * kElems;
              long long elemBytes = getElementBytes(outTy.getElementType());
              long long inElems = 1; for (int64_t d : inTy.getShape()) inElems *= d;
              long long filElems = 1; for (int64_t d : filTy.getShape()) filElems *= d;
              currentOpBytes = elemBytes * (inElems + filElems + outElems);
            })
            .Case<linalg::PoolingNchwSumOp, linalg::PoolingNchwMaxOp>([&](auto p) {
              if (p->getNumResults() == 0) return;
              auto outTy = dyn_cast<RankedTensorType>(p->getResult(0).getType());
              if (!outTy || !outTy.hasStaticShape() ||
                  !isa<FloatType>(outTy.getElementType()))
                return;
              long long outElems = 1;
              for (int64_t d : outTy.getShape())
                outElems *= (d == ShapedType::kDynamic ? 1 : d);
              auto kTy = dyn_cast<RankedTensorType>(p->getOperand(1).getType());
              if (kTy && kTy.hasStaticShape()) {
                long long kElems = 1;
                for (int64_t d : kTy.getShape())
                  kElems *= (d == ShapedType::kDynamic ? 1 : d);
                currentOpFlops = outElems * kElems;
                long long elemBytes = getElementBytes(outTy.getElementType());
                currentOpBytes = elemBytes * (outElems + kElems);
              } else {
                currentOpFlops = outElems;
                long long elemBytes = getElementBytes(outTy.getElementType());
                currentOpBytes = elemBytes * outElems * 2; // approx
              }
            })
            .Case<linalg::PoolingNhwcSumOp, linalg::PoolingNhwcMaxOp,
                  linalg::PoolingNhwcMinOp>([&](auto p) {
              auto outTy = dyn_cast<RankedTensorType>(p->getResult(0).getType());
              auto kTy   = dyn_cast<RankedTensorType>(p.getInputs()[1].getType());
              if (!outTy || !kTy || !outTy.hasStaticShape() || !kTy.hasStaticShape())
                return;
              long long outElems = 1, kElems = 1;
              for (int64_t d : outTy.getShape())
                outElems *= (d == ShapedType::kDynamic ? 1 : d);
              for (int64_t d : kTy.getShape())
                kElems *= (d == ShapedType::kDynamic ? 1 : d);
              currentOpFlops = outElems * kElems;
              long long elemBytes = getElementBytes(outTy.getElementType());
              currentOpBytes = elemBytes * (outElems + kElems);
            })
            .Case<linalg::FillOp>([&](auto) { currentOpFlops = 1; })
            .Default([&](Operation *op) {
              if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("flow.debug.flops"))
                currentOpFlops = flopsAttr.getInt();
              if (auto bytesAttr = op->getAttrOfType<IntegerAttr>("flow.debug.bytes"))
                currentOpBytes = bytesAttr.getInt();
            });

        // 간단한 시간 모델을 사용하여 기대 이득(Benefit)을 계산
        if (currentOpFlops > 0) {
          // 하드웨어 파라미터: 파일-스코프 llvm::cl::opt에서 읽습니다.
          const double P_cpu = pCpu.getValue();    // FLOP/s
          const double BW_cpu = bwCpu.getValue();  // bytes/s
          const double P_acc = pAcc.getValue();    // FLOP/s
          const double BW_acc = bwAcc.getValue();  // bytes/s
          const double BW_h2d = bwH2D.getValue();  // host->device bytes/s
          const double BW_d2h = bwD2H.getValue();  // device->host bytes/s
          const double launch_overhead = launchOverhead.getValue(); // seconds

          double t_cpu = 0.0, t_acc = 0.0;
          if (currentOpBytes > 0) {
            t_cpu = std::max((double)currentOpFlops / P_cpu, (double)currentOpBytes / BW_cpu);
            t_acc = std::max((double)currentOpFlops / P_acc, (double)currentOpBytes / BW_acc) +
                    (double)currentOpBytes / BW_h2d + (double)currentOpBytes / BW_d2h + launch_overhead;
          } else {
            // bytes 정보가 없으면 단순 FLOPs 기반 추정(전송 무시)
            t_cpu = (double)currentOpFlops / P_cpu;
            t_acc = (double)currentOpFlops / P_acc + launch_overhead;
          }
          currentOpBenefit = t_cpu - t_acc;

          op->setAttr("flow.debug.flops",
                      builder.getI64IntegerAttr(currentOpFlops));
          if (currentOpBytes > 0) {
            op->setAttr("flow.debug.bytes",
                        builder.getI64IntegerAttr(currentOpBytes));
          }
          op->setAttr("flow.debug.benefit",
                      builder.getF64FloatAttr(currentOpBenefit));

          // 수집: FLOPs 기반 리스트(이전 동작 유지)와 메트릭 리스트 병행 저장
          allOpsWithFlops.push_back({currentOpFlops, op});
          allOpsWithMetrics.push_back(std::make_tuple(currentOpFlops, currentOpBytes, currentOpBenefit, op));
        }
      });
    }

    // 안전 가드: 선택할 대상이 없음
    if (allOpsWithMetrics.empty()) {
      LLVM_DEBUG(llvm::dbgs() << "No operations with metrics found.\n");
      return;
    }

    // [2단계] Benefit 기반 선택: allOpsWithMetrics를 benefit 기준으로 정렬
    std::stable_sort(allOpsWithMetrics.begin(), allOpsWithMetrics.end(),
                     [](const auto &a, const auto &b) {
                       return std::get<2>(a) > std::get<2>(b); // benefit 내림차순
                     });

    // 긍정적 benefit만 수집 (benefit > 0인 연산이 오프로딩 대상)
    SmallVector<Operation *> opsWithPositiveBenefit;
    double totalPositiveBenefit = 0.0;
    for (auto &metric : allOpsWithMetrics) {
      double benefit = std::get<2>(metric);
      if (benefit > 0.0) {
        opsWithPositiveBenefit.push_back(std::get<3>(metric));
        totalPositiveBenefit += benefit;
      }
    }

    // 만약 긍정적 benefit이 하나도 없으면 아무 것도 선택하지 않음
    if (opsWithPositiveBenefit.empty()) {
      LLVM_DEBUG(llvm::dbgs() << "No operations with positive benefit (> 0). "
                              << "Nothing to offload.\n");
      return;
    }

    // ratio에 따른 누적 benefit 기반 Top-K 결정
    double ratio = std::clamp<double>(topKRatio, 0.0, 1.0);
    double targetBenefit = totalPositiveBenefit * ratio;

    double accBenefit = 0.0;
    int topNCount = 0;
    
    // allOpsWithMetrics는 benefit 내림차순으로 정렬되어 있음
    // 양수 benefit ops를 순회하며 targetBenefit 도달할 때까지 선택
    for (; topNCount < (int)opsWithPositiveBenefit.size(); ++topNCount) {
      double benefit = std::get<2>(allOpsWithMetrics[topNCount]);
      // 여기서는 benefit > 0인 ops만 처리 (opsWithPositiveBenefit에서 이미 필터링됨)
      
      accBenefit += benefit;
      if (accBenefit >= targetBenefit) {
        ++topNCount;
        break;
      }
    }

    LLVM_DEBUG(llvm::dbgs() << "Selected top " << topNCount << " of "
                            << opsWithPositiveBenefit.size()
                            << " ops with positive benefit for offloading.\n");
    LLVM_DEBUG(llvm::dbgs() << "Total positive benefit: " << totalPositiveBenefit
                            << "s, Target benefit (ratio=" << ratio
                            << "): " << targetBenefit << "s\n");

    // 3) [2단계] 선택된 Top-K 연산에 `offload.anchor` 속성을 부착합니다.
  // Create the target attribute from the configurable target name.
  auto sa16TargetName = builder.getStringAttr(accelTargetName.getValue());
    
    for (int i = 0; i < topNCount; ++i) {
      Operation *anchorOp = std::get<3>(allOpsWithMetrics[i]);
      anchorOp->setAttr("offload.anchor", builder.getUnitAttr());
      anchorOp->setAttr("offload.target", sa16TargetName);
      LLVM_DEBUG(llvm::dbgs() << "=> Marked anchor op (benefit=" << std::get<2>(allOpsWithMetrics[i])
                              << "s): " << *anchorOp << "\n");
    }

  }
};

} // namespace

std::unique_ptr<Pass> createIntelligentFusionPlannerPass() {
  return std::make_unique<IntelligentFusionPlannerPass>();
}
std::unique_ptr<Pass> createIntelligentFusionPlannerPass(double ratio) {
  return std::make_unique<IntelligentFusionPlannerPass>(ratio);
}

} // namespace mlir::iree_compiler::IREE::Flow
