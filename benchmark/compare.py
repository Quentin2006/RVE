#!/usr/bin/env python3
"""Compare the complete baseline and BVH benchmark matrices."""

import argparse
import math
from pathlib import Path
import statistics
import sys


MATERIALS = ("lambertian", "metal", "dielectric", "emissive", "random")
DENSITIES = ("full", "sparse")
SIZES = ("5x5x5", "10x10x10")
CONFIGS = tuple(
    Path(material) / density / size
    for material in MATERIALS
    for density in DENSITIES
    for size in SIZES
)


def load_results(directory: Path) -> dict[Path, float]:
    if not directory.is_dir():
        raise ValueError(f"not a results directory: {directory}")
    actual = {path.relative_to(directory).parent for path in directory.rglob("fps.txt")}
    expected = set(CONFIGS)
    if actual != expected:
        missing = sorted(str(path) for path in expected - actual)
        extra = sorted(str(path) for path in actual - expected)
        raise ValueError(f"{directory}: missing configurations {missing}; extra {extra}")

    results = {}
    for config in CONFIGS:
        path = directory / config / "fps.txt"
        try:
            values = [float(line) for line in path.read_text(encoding="utf-8").splitlines()]
        except ValueError as error:
            raise ValueError(f"{path}: invalid FPS value") from error
        if len(values) != 5 or any(not math.isfinite(value) or value <= 0 for value in values):
            raise ValueError(f"{path}: expected five finite, positive FPS samples")
        results[config] = statistics.mean(values)
    return results


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline", type=Path)
    parser.add_argument("bvh", type=Path)
    args = parser.parse_args()

    try:
        baseline = load_results(args.baseline)
        bvh = load_results(args.bvh)
    except (OSError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print("# BVH benchmark comparison")
    print()
    print(f"Baseline: `{args.baseline}`")
    print()
    print(f"BVH: `{args.bvh}`")
    print()
    print("| Material | Density | Size | Baseline mean FPS | BVH mean FPS | Speedup |")
    print("| --- | --- | --- | ---: | ---: | ---: |")
    speedups = []
    for config in CONFIGS:
        speedup = bvh[config] / baseline[config]
        speedups.append(speedup)
        material, density, size = config.parts
        print(
            f"| {material} | {density} | {size} | {baseline[config]:.4f} | "
            f"{bvh[config]:.4f} | {speedup:.4f}x |"
        )
    print()
    print(f"Minimum speedup: {min(speedups):.4f}x")
    print()
    print(f"Median speedup: {statistics.median(speedups):.4f}x")
    print()
    print(f"Maximum speedup: {max(speedups):.4f}x")
    return 0


if __name__ == "__main__":
    sys.exit(main())
