#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# Fusion sweep runner
# -----------------------------------------------------------------------------
# This script benchmarks the MobileNetV2 model for multiple top-k ratios while
# toggling the custom fusion pass on/off. Results are written to JSON files and
# companion PNG plots per ratio under ../results/mobilenetv2.
# -----------------------------------------------------------------------------

# Find IREE_BUILD_DIR automatically if not set
if [[ -z "${IREE_BUILD_DIR:-}" ]]; then
  # Try to find it relative to this script's location
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  IREE_BUILD_DIR="$(cd "${SCRIPT_DIR}/../../../../../iree-build" 2>/dev/null && pwd)" || \
  IREE_BUILD_DIR="$(cd "${SCRIPT_DIR}/../../../../../iree/iree-build" 2>/dev/null && pwd)" || \
  IREE_BUILD_DIR="/home/jueun/jux/project/iree/iree-build"
fi
: "${IREE_PATH:=${IREE_BUILD_DIR}/tools}"
: "${DEVICE_SA16:=accel}"
: "${RESULTS_ROOT:=../results}"
: "${MODEL_NAME:=mobilenetv2}"
: "${MODEL_SRC_DIR:=./model}"

MODEL_DIR="${RESULTS_ROOT}/${MODEL_NAME}"
MODEL_FILE="${MODEL_SRC_DIR}/${MODEL_NAME}.mlir"

# Allow overriding ratios/fusion modes via env vars, e.g.
#   RATIOS="0.1 0.5 1.0" FUSION_MODES="disabled enabled" ./run_fusion_sweep.sh
RATIOS_ENV="${RATIOS:-0.1 0.3 0.5 0.8 1.0}"
# RATIOS_ENV="${RATIOS:-0.8}"
# FUSION_MODES_ENV="${FUSION_MODES:-disabled enabled}"
FUSION_MODES_ENV="${FUSION_MODES:-enabled}"

read -r -a RATIOS_ARRAY <<< "${RATIOS_ENV}"
read -r -a FUSION_MODES_ARRAY <<< "${FUSION_MODES_ENV}"

# Optional debug knobs (mirrors run.sh)
: "${ENABLE_DEBUG:=0}"
DEBUG_ONLY_PASS="iree-flow-intelligent-fusion-planner"
DEBUG_FLAGS=()
if [[ "${ENABLE_DEBUG}" != "0" ]]; then
  DEBUG_FLAGS+=("-debug" "-debug-only=${DEBUG_ONLY_PASS}")
fi

# Skip baseline if precomputed and SKIP_BASELINE=1
: "${SKIP_BASELINE:=0}"

# Ensure output directory exists
mkdir -p "${MODEL_DIR}"

run_and_log() {
  local log="$1"
  shift
  {
    "$@" > >(tee "${log}") 2> >(tee -a "${log}" >&2)
  } || {
    local st=$?
    echo "❌ failure: $* (exit ${st})"
    echo "----- ${log} (tail 200) -----"
    tail -n 200 "${log}" || true
    exit "${st}"
  }
}

if [[ ! -x "${IREE_PATH}/iree-compile" ]]; then
  echo "iree-compile not found at ${IREE_PATH}/iree-compile"
  echo "Set IREE_PATH to the directory containing iree-compile / iree-benchmark-module."
  exit 1
fi

if [[ ! -x "${IREE_PATH}/iree-benchmark-module" ]]; then
  echo "iree-benchmark-module not found at ${IREE_PATH}/iree-benchmark-module"
  exit 1
fi

if [[ ! -f "${MODEL_FILE}" ]]; then
  echo "Model MLIR not found: ${MODEL_FILE}"
  exit 1
fi

BASE_VMFB="${MODEL_DIR}/${MODEL_NAME}_llvm-cpu.vmfb"
BASE_COMPILE_LOG="${MODEL_DIR}/compile_llvm_cpu.log"
BASE_BENCH_JSON="${MODEL_DIR}/llvm-cpu_results.json"

if [[ "${SKIP_BASELINE}" == "0" ]]; then
  echo "=== Baseline: llvm-cpu ==="
  run_and_log "${BASE_COMPILE_LOG}" \
    "${IREE_PATH}/iree-compile" \
      --iree-hal-target-backends=llvm-cpu \
      --iree-llvmcpu-target-cpu=host \
      --iree-dispatch-creation-enable-aggressive-fusion \
      "${MODEL_FILE}" \
      --iree-opt-level=O3 \
      "${DEBUG_FLAGS[@]}" \
      -o "${BASE_VMFB}"

  run_and_log "${MODEL_DIR}/benchmark_llvm_cpu.log" \
    "${IREE_PATH}/iree-benchmark-module" \
      --device=local-sync \
      --module="${BASE_VMFB}" \
      --function="torch-jit-export" \
      --input="1x3x224x224xf32" \
      --benchmark_out_format=json \
      --benchmark_repetitions=20 \
      --benchmark_out="${BASE_BENCH_JSON}"
else
  echo "Skipping baseline (SKIP_BASELINE=${SKIP_BASELINE})."
  if [[ ! -f "${BASE_BENCH_JSON}" ]]; then
    echo "Required baseline JSON missing: ${BASE_BENCH_JSON}"
    exit 1
  fi
fi

echo ""
echo "=== sa16 sweep: fusion on/off ==="

for ratio in "${RATIOS_ARRAY[@]}"; do
  # [수정] ratio에 상관없이 항상 원본 MLIR 파일을 사용하도록 변경
  # FLOW_FILE="${MODEL_SRC_DIR}/${MODEL_NAME}_flow_${ratio}.mlir"
  # if [[ ! -f "${FLOW_FILE}" ]]; then
  #   echo "Flow MLIR not found for ratio ${ratio}: ${FLOW_FILE}"
  #   exit 1
  # fi

  INPUT_MLIR="${MODEL_FILE}" # 원본 모델 파일 경로
  declare -a ratio_outputs=()

  for fusion_mode in "${FUSION_MODES_ARRAY[@]}"; do
    case "${fusion_mode}" in
      disabled) fusion_tag="nofusion" ;;
      enabled) fusion_tag="fusion" ;;
      *)
        fusion_tag="${fusion_mode//[^A-Za-z0-9]/_}"
        ;;
    esac

    SA16_VMFB="${MODEL_DIR}/${MODEL_NAME}_sa16_${ratio}_${fusion_tag}.vmfb"
    SA16_LOG="${MODEL_DIR}/compile_sa16_${ratio}_${fusion_tag}.log"
    SA16_JSON="${MODEL_DIR}/sa16_${ratio}_${fusion_tag}_results.json"
    BENCH_LOG="${MODEL_DIR}/benchmark_sa16_${ratio}_${fusion_tag}.log"

    echo "--- sa16 compile (ratio=${ratio}, fusion=${fusion_mode}) ---"

    # --mlir-print-ir-before=iree-hal-materialize-interfaces
    # --mlir-print-ir-module-scope
    COMPILE_CMD=(
      "${IREE_PATH}/iree-compile"
      --iree-hal-target-backends=sa16
      --iree-sa16-target-cpu=host
      --mlir-disable-threading
      "${INPUT_MLIR}" 
      --iree-opt-level=O3
    )

    if [[ "${fusion_mode}" == "enabled" ]]; then
      COMPILE_CMD+=(--iree-flow-enable-custom-fusion)
    # else
    #   COMPILE_CMD+=(--iree-flow-enable-custom-fusion=false)
    fi

    if [[ "${#DEBUG_FLAGS[@]}" -gt 0 ]]; then
      COMPILE_CMD+=("${DEBUG_FLAGS[@]}")
    fi

    COMPILE_CMD+=(-o "${SA16_VMFB}")

    run_and_log "${SA16_LOG}" \
      "${COMPILE_CMD[@]}"

    echo "--- sa16 benchmark (ratio=${ratio}, fusion=${fusion_mode}) ---"
    run_and_log "${BENCH_LOG}" \
      env \
        SA16_FAKE_RATIO="${ratio}" \
        ACCEL_ATTR_ONLY=1 \
        "${IREE_PATH}/iree-benchmark-module" \
          --device="${DEVICE_SA16}" \
          --module="${SA16_VMFB}" \
          --function="torch-jit-export" \
          --input="1x3x224x224xf32" \
          --benchmark_out_format=json \
          --benchmark_repetitions=10 \
          --benchmark_out="${SA16_JSON}"

    ratio_outputs+=("${SA16_JSON}")
  done

  # echo "--- plotting fusion comparison (ratio=${ratio}) ---"
  # python3 ./plot_results.py \
  #   "${BASE_BENCH_JSON}" \
  #   "${ratio_outputs[@]}" \
  #   --model_name "MobileNetV2 (ratio ${ratio})" \
  #   --output "${MODEL_DIR}/${MODEL_NAME}_fusion_compare_${ratio}.png"
done

echo ""
echo "Fusion sweep complete. Inspect JSON/PNG artifacts under ${MODEL_DIR}"
