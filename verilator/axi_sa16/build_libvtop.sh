#!/usr/bin/env bash
set -euo pipefail

TOP="axi_sa16"
SRC_DIR="src"
OBJ_DIR="obj_dir"
OUT_DIR="build"
OUT_SO="${OUT_DIR}/libVtop.so"
SIM_CPP="sim_lib.cpp"   # 네 DPI/래퍼 파일명

CXX="${CXX:-g++}"
CXXSTD="-std=c++17"
VM_TRACE="${VM_TRACE:-0}"   # 1이면 VCD 트레이스 활성화

# VERILATOR_ROOT 자동 감지
if [[ -z "${VERILATOR_ROOT:-}" ]]; then
  if command -v verilator >/dev/null 2>&1; then
    VERILATOR_ROOT="$(verilator --getenv VERILATOR_ROOT)"
  fi
fi
: "${VERILATOR_ROOT:?Set VERILATOR_ROOT or ensure 'verilator' is on PATH}"

INCLUDE_VLT="${VERILATOR_ROOT}/include"
INCLUDE_VLTSTD="${VERILATOR_ROOT}/include/vltstd"

mkdir -p "${OBJ_DIR}" "${OUT_DIR}"

# filelist.f 자동 생성
FILELIST="filelist.f"
find "${SRC_DIR}" -maxdepth 1 -type f \( -name "*.v" -o -name "*.sv" \) | sort > "${FILELIST}"

echo "[build] verilating..."
verilator -cc --sv -O3 \
  --Mdir "${OBJ_DIR}" \
  --top-module "${TOP}" \
  -f "${FILELIST}" \
  -CFLAGS "-O3 -fPIC" \
  -LDFLAGS "-fPIC" \
  -Wno-UNOPTFLAT -Wno-CASEINCOMPLETE -Wno-WIDTH -Wno-PINCONNECTEMPTY 

echo "[link] ${OUT_SO}"

TRACE_DEFINE=""
if [[ "${VM_TRACE}" == "1" ]]; then
  TRACE_DEFINE="-DVM_TRACE=1"
fi

# 스레드 미사용: verilated_threads.cpp 링크하지 않음!
"${CXX}" -shared -o "${OUT_SO}" -fPIC -O3 ${TRACE_DEFINE} -pthread \
  -I"${OBJ_DIR}" \
  -I"${INCLUDE_VLT}" \
  -I"${INCLUDE_VLTSTD}" \
  "${SIM_CPP}" \
  "${OBJ_DIR}/V${TOP}__ALL.a" \
  "${INCLUDE_VLT}/verilated.cpp"

echo "[done] -> ${OUT_SO}"
echo "export VTOP_LIBRARY_PATH=$(readlink -f "${OUT_SO}")"
