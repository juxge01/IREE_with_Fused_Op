// CalculateDispatchDataTransferPass.cpp
// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_CALCULATEDISPATCHDATATRANSFERPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

static int64_t getTensorSizeInBytes(RankedTensorType tensorType) {
  int64_t elemBits = tensorType.getElementTypeBitWidth();
  if (!tensorType.hasStaticShape()) {
    int64_t numElements = 1;
    for (int64_t dim : tensorType.getShape()) {
      if (ShapedType::isDynamic(dim)) continue;  // 동적=1 가정
      numElements *= dim;
    }
    if (numElements <= 0) return 0;
    return (numElements * elemBits) / 8;
  }
  return (tensorType.getNumElements() * elemBits) / 8;
}

static bool isRankedTensor(Value v, RankedTensorType &outTy) {
  if (auto ty = llvm::dyn_cast<RankedTensorType>(v.getType())) {
    outTy = ty;
    return true;
  }
  return false;
}

/// Dispatch-like op 하나에 대해 H2D/D2H 바이트 수를 계산해 attr로 기록.
template <typename DispatchLikeOp>
static void annotateOneDispatch(DispatchLikeOp dispatchOp, OpBuilder &builder) {
  int64_t bytesH2D = 0;
  int64_t bytesD2H = 0;

  llvm::DenseSet<Value> seenH2D, seenD2H;

  // 입력(H2D): 오퍼랜드 텐서 크기 합산
  for (Value v : dispatchOp->getOperands()) {
    RankedTensorType rt;
    if (!isRankedTensor(v, rt)) continue;
    if (seenH2D.insert(v).second) bytesH2D += getTensorSizeInBytes(rt);
  }

  // 출력(D2H): 결과 텐서 크기 합산
  for (Value v : dispatchOp->getResults()) {
    RankedTensorType rt;
    if (!isRankedTensor(v, rt)) continue;
    if (seenD2H.insert(v).second) bytesD2H += getTensorSizeInBytes(rt);
  }

  dispatchOp->setAttr("bytes_h2d", builder.getI64IntegerAttr(bytesH2D));
  dispatchOp->setAttr("bytes_d2h", builder.getI64IntegerAttr(bytesD2H));
}

/// 내부 오프로딩(클러스터) 케이스:
/// - 같은 `cluster.id`를 가진 op들을 하나의 클러스터로 보고,
/// - 경계 기준으로 H2D/D2H를 계산:
///   * H2D: 클러스터 바깥에서 들어오는 피연산 텐서
///   * D2H: 클러스터 안에서 만들어져 바깥에서 사용되는 결과 텐서
struct ClusterTransferCalculator {
  OpBuilder &builder;

  struct Totals {
    int64_t h2d = 0;
    int64_t d2h = 0;
  };

  using OpSet = llvm::SmallPtrSet<Operation *, 32>;

  static void collectClusters(func::FuncOp funcOp,
                              llvm::DenseMap<int64_t, llvm::SmallVector<Operation *>> &clusters) {
    funcOp.walk([&](Operation *op) {
      if (auto attr = op->getAttrOfType<IntegerAttr>("cluster.id")) {
        clusters[attr.getInt()].push_back(op);
      }
    });
  }

  static Operation *chooseAnchor(const llvm::SmallVector<Operation *> &ops) {
    // offload.anchor 가 있으면 그 op 를 우선.
    for (Operation *op : ops) {
      if (auto anchor = op->getAttrOfType<IntegerAttr>("offload.anchor")) {
        if (anchor.getInt() != 0) return op;
      }
      if (auto anchorB = op->getAttrOfType<BoolAttr>("offload.anchor")) {
        if (anchorB.getValue()) return op;
      }
    }
    // 없으면 첫 op를 대표로.
    return ops.empty() ? nullptr : ops.front();
  }

  static OpSet makeOpSet(const llvm::SmallVector<Operation *> &ops) {
    OpSet s;
    for (auto *op : ops) s.insert(op);
    return s;
  }

  static bool valueDefinedInsideCluster(Value v, const OpSet &clusterOps) {
    if (auto *defOp = v.getDefiningOp()) return clusterOps.contains(defOp);
    // 블록 인자라면 클러스터 밖에서 온다고 간주 (H2D 후보)
    return false;
  }

  static bool anyUserOutsideCluster(Value v, const OpSet &clusterOps) {
    for (Operation *user : v.getUsers()) {
      if (!clusterOps.contains(user)) return true;
    }
    // 유저가 없으면 “바깥으로 나간다”로 보긴 애매하지만, 기본은 false로 두자.
    return false;
  }

  Totals computeForCluster(const llvm::SmallVector<Operation *> &ops) {
    Totals totals;
    if (ops.empty()) return totals;

    OpSet opSet = makeOpSet(ops);
    llvm::DenseSet<Value> seenH2D, seenD2H;

    // H2D: 클러스터 안 op들의 피연산 중, 정의가 클러스터 밖인 텐서
    for (Operation *op : ops) {
      for (Value operand : op->getOperands()) {
        RankedTensorType rt;
        if (!isRankedTensor(operand, rt)) continue;
        if (!valueDefinedInsideCluster(operand, opSet)) {
          if (seenH2D.insert(operand).second) {
            totals.h2d += getTensorSizeInBytes(rt);
          }
        }
      }
    }

    // D2H: 클러스터 안에서 정의된 결과 중, 사용자 중 바깥 사용자가 있는 텐서
    for (Operation *op : ops) {
      for (Value result : op->getResults()) {
        RankedTensorType rt;
        if (!isRankedTensor(result, rt)) continue;
        if (anyUserOutsideCluster(result, opSet)) {
          if (seenD2H.insert(result).second) {
            totals.d2h += getTensorSizeInBytes(rt);
          }
        }
      }
    }

    return totals;
  }

  void annotateCluster(Operation *anchor, const Totals &totals) {
    if (!anchor) return;
    anchor->setAttr("bytes_h2d", builder.getI64IntegerAttr(totals.h2d));
    anchor->setAttr("bytes_d2h", builder.getI64IntegerAttr(totals.d2h));
  }
};

struct CalculateDispatchDataTransferPass
    : public IREE::Flow::impl::CalculateDispatchDataTransferPassBase<
          CalculateDispatchDataTransferPass> {
  void runOnOperation() override {
    func::FuncOp funcOp = getOperation();
    OpBuilder builder(funcOp.getContext());

    // 1) dispatch 케이스 (있으면 그대로 annotate)
    bool sawDispatch = false;
    funcOp.walk([&](IREE::Flow::DispatchOp op) {
      annotateOneDispatch(op, builder);
      sawDispatch = true;
    });
    funcOp.walk([&](IREE::Flow::DispatchWorkgroupsOp op) {
      annotateOneDispatch(op, builder);
      sawDispatch = true;
    });
    if (sawDispatch) return;

    // 2) 내부 오프로딩(클러스터) 케이스
    llvm::DenseMap<int64_t, llvm::SmallVector<Operation *>> clusters;
    ClusterTransferCalculator::collectClusters(funcOp, clusters);
    if (clusters.empty()) return;

    ClusterTransferCalculator calc{builder};
    for (auto &it : clusters) {
      auto &ops = it.second;
      auto totals = calc.computeForCluster(ops);
      Operation *anchor = ClusterTransferCalculator::chooseAnchor(ops);
      calc.annotateCluster(anchor, totals);
    }
  }
};

}  // namespace

std::unique_ptr<Pass> createCalculateDispatchDataTransferPass() {
  return std::make_unique<CalculateDispatchDataTransferPass>();
}

}  // namespace mlir::iree_compiler::IREE::Flow
