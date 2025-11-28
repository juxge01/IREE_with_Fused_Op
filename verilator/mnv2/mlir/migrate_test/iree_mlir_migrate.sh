#!/usr/bin/env bash
# iree_mlir_migrate.sh
# Usage:
#   ./iree_mlir_migrate.sh                # 현재 디렉토리의 모든 *.mlir 변환
#   ./iree_mlir_migrate.sh path/a.mlir b.mlir subdir/*.mlir
#
# NaN 처리 의미 바꾸고 싶으면:
#   MODE=num    # arith.minnumf / arith.maxnumf 로 바꿈 (NaN 무시)
#   MODE=propagate (기본)  # arith.minimumf / arith.maximumf (NaN 전파)

set -euo pipefail

MODE="${MODE:-propagate}"

# 파일 리스트 수집
if [ "$#" -gt 0 ]; then
  FILES=("$@")
else
  mapfile -t FILES < <(find . -type f -name '*.mlir' | sort)
fi

if [ "${#FILES[@]}" -eq 0 ]; then
  echo "변환할 .mlir 파일이 없음"
  exit 0
fi

for f in "${FILES[@]}"; do
  [ -f "$f" ] || continue
  cp -- "$f" "$f.bak"

  sed -E -i 's/\bflow\.dispatch\.workgroup_count_from_dag_root\b/iree_tensor_ext.dispatch.workgroup_count_from_dag_root/g' "$f"

  sed -E -i '/iree_tensor_ext\.dispatch\.workgroup_count_from_dag_root[[:space:]]*\(/! s/(iree_tensor_ext\.dispatch\.workgroup_count_from_dag_root)[[:space:]]+([^()]+)$/\1(\2)/' "$f"

  sed -E -i 's/!flow\.dispatch\.tensor</!iree_tensor_ext.dispatch.tensor</g' "$f"
  sed -E -i 's/\bflow\.dispatch\.tensor\./iree_tensor_ext.dispatch.tensor./g' "$f"

  if [ "$MODE" = "num" ]; then
    sed -E -i 's/\barith\.minf\b/arith.minnumf/g' "$f"
    sed -E -i 's/\barith\.maxf\b/arith.maxnumf/g' "$f"
  else
    sed -E -i 's/\barith\.minf\b/arith.minimumf/g' "$f"
    sed -E -i 's/\barith\.maxf\b/arith.maximumf/g' "$f"
  fi

  # grep -nE 'flow\.dispatch\.|arith\.minf|arith\.maxf' "$f" || true

  echo "✔ 변환 완료: $f  (백업: $f.bak)"
done

