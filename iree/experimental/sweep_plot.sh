#!/usr/bin/env bash
set -euo pipefail

# -----------------------------------------------------------------------------
# Fusion sweep plotter (modified for single combined plot)
# -----------------------------------------------------------------------------
# This script plots the pre-computed benchmark results for MobileNetV2,
# combining all ratios into a single comparison plot.
# Assumes JSON files already exist under ../results/mobilenetv2.
# -----------------------------------------------------------------------------

: "${IREE_PATH:=${HOME}/.local/bin}"
: "${DEVICE_SA16:=accel}"
: "${RESULTS_ROOT:=../results}"
: "${MODEL_NAME:=mobilenetv2}"
: "${MODEL_SRC_DIR:=./model}"

MODEL_DIR="${RESULTS_ROOT}/${MODEL_NAME}"

RATIOS_ENV="${RATIOS:-0.1 0.3 0.5 0.8 1.0}"
# RATIOS_ENV="${RATIOS:-1.0}" 
FUSION_MODES_ENV="${FUSION_MODES:-enabled}"
# FUSION_MODES_ENV="${FUSION_MODES:-disabled enabled}"
read -r -a RATIOS_ARRAY <<< "${RATIOS_ENV}"
read -r -a FUSION_MODES_ARRAY <<< "${FUSION_MODES_ENV}"

# Ensure output directory exists
mkdir -p "${MODEL_DIR}"

BASE_BENCH_JSON="${MODEL_DIR}/llvm-cpu_results.json"
if [[ ! -f "${BASE_BENCH_JSON}" ]]; then
  echo "Required baseline JSON missing: ${BASE_BENCH_JSON}"
  exit 1
fi

if [[ ! -f "./plot_results.py" ]]; then
    echo "Plotting script not found: ./plot_results.py"
    exit 1
fi

echo "=== Collecting sa16 sweep result file paths ==="


declare -a all_sa16_outputs=()

for ratio in "${RATIOS_ARRAY[@]}"; do
  for fusion_mode in "${FUSION_MODES_ARRAY[@]}"; do
    case "${fusion_mode}" in
      disabled) fusion_tag="nf" ;;
      enabled) fusion_tag="f" ;;
      *)
        fusion_tag="${fusion_mode//[^A-Za-z0-9]/_}"
        ;;
    esac

    SA16_JSON="${MODEL_DIR}/sa16_${ratio}_${fusion_tag}_results.json"

    if [[ ! -f "${SA16_JSON}" ]]; then
        echo "Required sa16 JSON missing for ratio=${ratio}, fusion=${fusion_mode}: ${SA16_JSON}"
        exit 1
    fi

    all_sa16_outputs+=("${SA16_JSON}")
  done
done

echo ""
echo "--- plotting combined fusion comparison (all ratios) ---"
python3 ./plot_results.py \
  "${BASE_BENCH_JSON}" \
  "${all_sa16_outputs[@]}" \
  --model_name "MobileNetV2 Fusion Comparison" \
  --output "${MODEL_DIR}/${MODEL_NAME}_fusion_compare_all_ratios.png"

echo ""
echo "Plotting complete. Inspect PNG artifact under ${MODEL_DIR}"
