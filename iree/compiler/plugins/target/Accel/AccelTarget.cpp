// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// --- VMVX의 헤더 목록을 그대로 사용하여 필요한 모든 파일을 포함합니다 ---
#include "iree/compiler/Codegen/Dialect/CPU/IR/IREECPUDialect.h"
#include "iree/compiler/Codegen/Dialect/CPU/IR/IREECPUTypes.h"
#include "iree/compiler/Codegen/Dialect/Codegen/IR/IREECodegenDialect.h"
#include "iree/compiler/Codegen/VMVX/Passes.h"
#include "iree/compiler/Dialect/HAL/Target/Devices/LocalDevice.h"
#include "iree/compiler/Dialect/HAL/Target/TargetRegistry.h"
#include "iree/compiler/Dialect/LinalgExt/IR/LinalgExtDialect.h"
#include "iree/compiler/Dialect/VM/Conversion/ConversionTarget.h"
#include "iree/compiler/Dialect/VM/IR/VMDialect.h"
#include "iree/compiler/Dialect/VM/Target/Bytecode/BytecodeModuleTarget.h"
#include "iree/compiler/Dialect/VM/Transforms/Passes.h"
#include "iree/compiler/Dialect/VMVX/IR/VMVXDialect.h"
#include "iree/compiler/Dialect/VMVX/Transforms/Passes.h"
#include "iree/compiler/PluginAPI/Client.h"
#include "llvm/Support/CommandLine.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/OperationSupport.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/LogicalResult.h"

#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"

namespace mlir::iree_compiler::IREE::HAL {

namespace {
// 옵션 구조체 이름을 ACCELOptions로 변경 (내용은 단순화를 위해 비움)
struct ACCELOptions {
  void bindOptions(OptionsBinder &binder) {
    static llvm::cl::OptionCategory category("ACCEL HAL Target");
    // TODO: 필요 시 여기에 ACCEL 백엔드 전용 옵션을 추가합니다.
  }
};
} // namespace

// ExecutableTarget을 생성하는 헬퍼 함수
static IREE::HAL::ExecutableTargetAttr
getACCELExecutableTarget(MLIRContext *context, StringRef backend,
                         StringRef format) {
  Builder b(context);
  SmallVector<NamedAttribute> configItems;
  // TODO: 필요 시 여기에 ACCEL 백엔드 고유의 설정 속성을 추가합니다.
  return b.getAttr<IREE::HAL::ExecutableTargetAttr>(
      b.getStringAttr(backend), b.getStringAttr(format),
      b.getDictionaryAttr(configItems));
}

// TargetBackend 클래스 이름을 ACCELTargetBackend로 변경
class ACCELTargetBackend final : public TargetBackend {
public:
  ACCELTargetBackend(const ACCELOptions &options) : options(options) {}

  std::string getLegacyDefaultDeviceID() const override { return "accel"; }

  // 기본 타겟을 'accel'로 설정
  void getDefaultExecutableTargets(
      MLIRContext *context, StringRef deviceID, DictionaryAttr deviceConfigAttr,
      SmallVectorImpl<IREE::HAL::ExecutableTargetAttr> &executableTargetAttrs)
      const override {
    executableTargetAttrs.push_back(getACCELExecutableTarget(
        context, "accel", "accel-bytecode-fb"));
  }

  // 필요한 Dialect 목록 (VMVX와 동일하게 유지하여 완벽한 동작 보장)
  void getDependentDialects(DialectRegistry &registry) const override {
    registry
        .insert<IREE::Codegen::IREECodegenDialect, IREE::CPU::IREECPUDialect,
                IREE::VM::VMDialect, IREE::VMVX::VMVXDialect,
                IREE::LinalgExt::IREELinalgExtDialect>();
  }

  IREE::VM::TargetOptions
  getTargetOptions(IREE::HAL::ExecutableTargetAttr targetAttr) {
    auto vmOptions = IREE::VM::TargetOptions::FromFlags::get();
    vmOptions.f32Extension = true;
    vmOptions.f64Extension = true;
    vmOptions.optimizeForStackSize = false;
    return vmOptions;
  }

  // === 파이프라인 설정 (VMVX와 동일하게 유지) ===
  // 이 부분들이 Linalg -> VM으로 변환하는 핵심 로직입니다.
  // 나중에 이 함수들을 직접 만든 패스로 교체하여 ACCEL 고유의 동작을 구현할 수 있습니다.

  void
  buildConfigurationPassPipeline(IREE::HAL::ExecutableTargetAttr targetAttr,
                                 OpPassManager &passManager) override {
    // TODO: 현재는 VMVX의 파이프라인을 그대로 사용합니다.
    //       나중에 이 부분을 ACCEL 백엔드 고유의 패스 파이프라인으로 교체해야 합니다.
    IREE::VMVX::buildVMVXConfigurationPassPipeline(passManager);
  }

  void buildTranslationPassPipeline(IREE::HAL::ExecutableTargetAttr targetAttr,
                                    OpPassManager &passManager) override {
    // 이 파이프라인은 hal.executable.variant 내부의 코드를 최종 타겟 형식(VM 바이트코드)으로
    // 변환하는 역할을 합니다.
    // VM 기반 백엔드이므로 VMVX의 번역 파이프라인을 시작점으로 사용합니다.
    //
    // 참고: 커스텀 퓨전 관련 패스(예: FlowTileFusedOpsPass)는
    // 이 파이프라인 이전에 Flow Dialect 단계에서
    // `--iree-flow-enable-custom-fusion`과 같은 플래그로 제어되어 실행됩니다.
    // 따라서 이 함수는 퓨전된 op이든 아니든 상관없이 들어오는 op들을
    // VMVX와 동일한 방식으로 타일링하고 코드 생성하는 역할을 합니다.

    IREE::VMVX::buildVMVXTransformPassPipeline(passManager);

    OpPassManager &nestedModulePM = passManager.nest<ModuleOp>();

    auto vmOptions = getTargetOptions(targetAttr);
    IREE::VM::buildVMTransformPassPipeline(nestedModulePM, vmOptions);
  }

  void buildLinkingPassPipeline(OpPassManager &passManager) override {
    buildVMVXLinkingPassPipeline(passManager);
  }

  // === 직렬화 (VMVX와 동일하게 유지) ===
  // VM 모듈을 최종 바이너리로 만드는 C++ 함수입니다.
  LogicalResult serializeExecutable(const SerializationOptions &serOptions,
                                    IREE::HAL::ExecutableVariantOp variantOp,
                                    OpBuilder &executableBuilder) override {
    auto vmModule =
        *variantOp.getInnerModule().getOps<IREE::VM::ModuleOp>().begin();
    SymbolTable symbolTable(vmModule);
    for (auto exportOp : variantOp.getBlock().getOps<ExecutableExportOp>()) {
      auto funcOp = symbolTable.lookup<IREE::VM::FuncOp>(exportOp.getName());

      // `local_memory`는 워크그룹 로컬 메모리 크기를 런타임에 알려줍니다.
      auto localMemorySizeAttr = exportOp.getWorkgroupLocalMemoryAttr();
      if (localMemorySizeAttr) {
        funcOp.setReflectionAttr("local_memory", localMemorySizeAttr);
      }

      // `binding_count`와 `constant_count`는 디스패치에 필요한 바인딩과
      // 푸시 상수 개수를 런타임에 알려주는 필수 속성입니다.
      auto layout = exportOp.getLayout();
      if (layout) {
        int64_t constantCount = layout.getConstants();
        if (constantCount > 0) {
          funcOp.setReflectionAttr("constant_count", executableBuilder.getI8IntegerAttr(constantCount));
        }
        size_t bindingCount = layout.getBindings().size();
        funcOp.setReflectionAttr("binding_count", executableBuilder.getI8IntegerAttr(bindingCount));
      }
    }

    SmallVector<char> moduleData;
    {
      auto vmOptions = getTargetOptions(variantOp.getTargetAttr());
      auto bytecodeOptions = IREE::VM::BytecodeTargetOptions::FromFlags::get();
      llvm::raw_svector_ostream stream(moduleData);
      if (failed(translateModuleToBytecode(variantOp.getInnerModule(),
                                           vmOptions, bytecodeOptions,
                                           stream))) {
        return variantOp.emitOpError()
               << "failed to serialize VM bytecode module";
      }
    }

    if (!serOptions.dumpBinariesPath.empty()) {
      dumpDataToPath<char>(serOptions.dumpBinariesPath, serOptions.dumpBaseName,
                           variantOp.getName(), ".vmfb", moduleData);
    }

    auto bufferAttr = DenseIntElementsAttr::get(
        VectorType::get({static_cast<int64_t>(moduleData.size())},
                        IntegerType::get(executableBuilder.getContext(), 8)),
        std::move(moduleData));

    auto binaryOp = executableBuilder.create<IREE::HAL::ExecutableBinaryOp>(
        variantOp.getLoc(), variantOp.getSymName(),
        variantOp.getTarget().getFormat(), bufferAttr);
    binaryOp.setMimeTypeAttr(
        executableBuilder.getStringAttr("application/x-flatbuffers"));

    return success();
  }

private:
  const ACCELOptions &options;
};


namespace {
// 플러그인 세션 이름을 ACCELSession으로 변경
struct ACCELSession
    : public PluginSession<ACCELSession, ACCELOptions,
                           PluginActivationPolicy::DefaultActivated> {
  void populateHALTargetDevices(IREE::HAL::TargetDeviceList &devices) {
    // "accel" 장치에 대한 컴파일러 측 표현을 등록합니다.
    // 'iree-compile'은 이 등록을 통해 'accel' 장치의 속성을 알 수 있습니다.
    // 현재 'accel' 백엔드는 VMVX와 유사하게 CPU에서 실행되므로,
    // 'local' 장치의 속성을 그대로 사용하도록 'LocalDevice'를 등록합니다.
    devices.add("accel", []() {
      IREE::HAL::LocalDevice::Options options;
      // 필요하다면 여기서 LocalDevice의 옵션을 맞춤 설정할 수 있습니다.
      return std::make_shared<IREE::HAL::LocalDevice>(options);
    });
  }

  void populateHALTargetBackends(IREE::HAL::TargetBackendList &targets) {
    // "vmvx" 대신 "accel"이라는 이름으로 백엔드를 등록합니다.
    targets.add("accel",
                [this]() { return std::make_shared<ACCELTargetBackend>(options); });
  }
};
} // namespace

} // namespace mlir::iree_compiler::IREE::HAL

// 플러그인 등록 함수 이름을 변경
extern "C" bool iree_register_compiler_plugin_hal_target_accel(
    mlir::iree_compiler::PluginRegistrar *registrar) {
  registrar->registerPlugin<mlir::iree_compiler::IREE::HAL::ACCELSession>(
      "hal_target_accel");
  return true;
}

// 옵션 플래그 정의 매크로의 이름을 변경
IREE_DEFINE_COMPILER_OPTION_FLAGS(mlir::iree_compiler::IREE::HAL::ACCELOptions);