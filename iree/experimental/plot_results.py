#!/usr/bin/env python3
import json
import argparse
import os
import matplotlib.pyplot as plt
import numpy as np


def parse_benchmark_result(file_path):
    with open(file_path, 'r') as f:
        data = json.load(f)

    if not data.get('benchmarks'):
        raise ValueError(f"'{file_path}' 파일에 'benchmarks' 키가 없습니다.")

    mean_benchmark = next((b for b in data['benchmarks'] if b.get('aggregate_name') == 'mean'), None)

    if mean_benchmark:
        benchmark_result = mean_benchmark
    else:

        benchmark_result = data['benchmarks'][0]

    real_time = benchmark_result.get('real_time')
    time_unit = benchmark_result.get('time_unit', 'ms')

    if real_time is None:
        raise ValueError(f"'{file_path}'의 벤치마크 결과에서 'real_time'을 찾을 수 없습니다.")

    return float(real_time), time_unit


def plot_comparison(result_files, model_name, output_file):
    try:
        # New behavior: treat first file as baseline (cpu) and group candidates by ratio
        baseline_file = result_files[0]
        candidate_files = result_files[1:]

        baseline_time, time_unit = parse_benchmark_result(baseline_file)

        # Parse candidates into a mapping: ratio -> {variant: time}
        results_by_ratio = {}
        for f in candidate_files:
            basename = os.path.basename(f)
            stem, _ = os.path.splitext(basename)
            parts = stem.split('_')
            # expected format: sa16_<ratio>_<variant>_results
            if len(parts) < 3:
                print(f"WARNNING: 파일명 형식 불명확, 건너뜁니다: {basename}")
                continue
            target = parts[0]
            ratio = parts[1]
            variant = parts[2]
            try:
                tval, _ = parse_benchmark_result(f)
            except Exception as e:
                print(f"Error: {f} 파싱 실패: {e}")
                continue
            results_by_ratio.setdefault(ratio, {})[variant] = tval

        if not results_by_ratio:
            print("Error: 후보 결과 파일이 없습니다.")
            return

        # Sort ratios numerically when possible
        def ratio_key(r):
            try:
                return float(r)
            except Exception:
                return r

        sorted_ratios = sorted(results_by_ratio.keys(), key=ratio_key)

        # Build grouped labels and times: for each ratio, we will plot cpu, nf, f
        labels = []
        times = []
        groups = []  # list of (ratio, [cpu,nf,f])
        for r in sorted_ratios:
            group = []
            # baseline (cpu) repeated per ratio for comparison
            group.append(baseline_time)
            # nf then f
            nf_t = results_by_ratio[r].get('nf') or results_by_ratio[r].get('nofusion')
            f_t = results_by_ratio[r].get('f') or results_by_ratio[r].get('fusion')
            group.append(nf_t if nf_t is not None else float('nan'))
            group.append(f_t if f_t is not None else float('nan'))
            groups.append((r, group))

        # Flatten labels and times for plotting grouped bars
        for r, grp in groups:
            labels.extend([f'{r}\ncpu', f'{r}\nnf', f'{r}\nf'])
            times.extend(grp)

    except (IOError, ValueError) as e:
        print(f"Error: 벤치마크 결과 파일을 처리하는 중 문제가 발생했습니다. ({e})")
        return

    x = np.arange(len(labels))
    fig, ax = plt.subplots(figsize=(12, 6))

    # 색상: cpu=blue, nf=orange, f=green (repeating per group)
    colors = []
    for _ in groups:
        colors.extend(['#1f77b4', '#ff7f0e', '#2ca02c'])

    bars = ax.bar(x, times, color=colors)

    ax.set_ylabel(f'Average Inference Time ({time_unit})')
    ax.set_title(f'{model_name} Performance: cpu vs nf vs f (by ratio)')
    ax.set_xticks(x)
    ax.set_xticklabels(labels, rotation=45, ha="right")

    # 표시: 전체 최솟값 라인
    finite_times = [t for t in times if not np.isnan(t)]
    if finite_times:
        best_time = float(np.min(finite_times))
        ax.axhline(y=best_time, color='g', linestyle='--', label=f'Best Time ({best_time:.2f})')
        ax.legend()

    # 각 막대 위에 실행 시간과 성능 향상 배수(Speedup) 표시
    for i, bar in enumerate(bars):
        yval = bar.get_height()
        if np.isnan(yval):
            continue
        ax.text(bar.get_x() + bar.get_width()/2.0, yval + 0.01 * max([t for t in times if not np.isnan(t)]), f'{yval:.2f}',
                va='bottom', ha='center')
        # speedup: only for nf and f (positions 1 and 2 in each group)
        group_idx = i % 3
        if group_idx in (1, 2):
            # corresponding baseline index is i - group_idx
            base_idx = i - group_idx
            base_time = times[base_idx]
            if not np.isnan(base_time) and yval > 0:
                speedup = base_time / yval
                ax.text(bar.get_x() + bar.get_width()/2.0, yval / 2, f'{speedup:.2f}x',
                        va='center', ha='center', color='white', fontsize=10, fontweight='bold')

    plt.tight_layout()
    plt.savefig(output_file)
    print(f"그래프가 '{output_file}' 파일로 저장되었습니다.")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Compare multiple IREE benchmark JSON results and generate a plot.'
    )
    parser.add_argument(
        'result_files',
        nargs='+',
        help='List of benchmark result JSON files. The first one is treated as the baseline (e.g., llvm-cpu).')
    parser.add_argument(
        '--model_name',
        type=str,
        default='Model',
        help='Name of the model for the plot title (e.g., "MobileNetV2").')
    parser.add_argument(
        '--output',
        type=str,
        default='benchmark_comparison.png',
        help='Output image file name.')

    args = parser.parse_args()
    plot_comparison(args.result_files, args.model_name, args.output)
