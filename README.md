
# RVE — Ray-Traced Voxel Engine

A CPU ray tracer written in C++20, with SDL2 for the window and pixel output
and GLM for vector and matrix math. The current implementation traces rays
against transformed cubes and supports Lambertian, metallic, dielectric, and
emissive materials. CUDA was explored in earlier commits; the current renderer
does not use CUDA or GPU ray tracing.

## Features

- Stochastic per-pixel sampling and recursive ray bounces.
- Mouse-look camera with keyboard movement through voxel scenes.
- Seeded world generation with selectable dimensions, material, and density.
- Loading scene files from disk.
- A Python benchmark driver that collects FPS samples, Linux perf recordings,
  and FlameGraph visualizations across a scene matrix.

## Requirements

- Linux with a graphical session and an SDL2 accelerated renderer.
- A C++20 compiler (`g++` by default), GNU Make, SDL2 development files, GLM
  headers, and Boost headers (`boost/algorithm/string.hpp`).
- Optional profiling tools: Python 3 with the `halo` package, Linux `perf`,
  `inxi`, and the `stackcollapse-perf.pl` and `flamegraph.pl` scripts from
  [FlameGraph](https://github.com/brendangregg/FlameGraph).

The Makefile assumes headers are installed under `/usr/include/SDL2` and
`/usr/include/glm`. Override `CPPFLAGS` for another installation layout.

## Build and run

```sh
make                          # optimized release build
./bin/rve --help              # list scene-generation options
./bin/rve --seed 42 --size 10x10x10 --material random --density sparse
./bin/rve --world benchmark/scenes/metal/sparse/5x5x5.scene
```

Build modes are `release` (default), `debug`, and `benchmark`. Clean before
switching modes because object filenames are shared:

```sh
make clean
make BUILD=debug
```

The release build uses `-march=native`; rebuild on the target machine rather
than distributing that binary across different CPUs.

## Controls

Mouse motion changes camera direction. `W`/`S` move forward/backward, `A`/`D`
move left/right, Space moves up, and Left Shift moves down. Close the window
to exit.

## Profiling

```sh
python3 benchmark/benchmark.py --seed 42
python3 benchmark/benchmark.py --material metal --density sparse --size 5x5x5
```

The driver rebuilds with profiling flags and collects five FPS samples per
run. Its default matrix has five material modes (the four materials plus
`random`), two density modes, and two active sizes (`5x5x5` and `10x10x10`):
20 runs. `50x50x50` is listed as an option but currently skipped by the driver
because of runtime cost. Results go under `benchmark/results/`.

The committed profiling results are historical artifacts, not portable
performance guarantees. Frame rates depend on the scene, renderer settings,
CPU, compiler, and profiling overhead. No automated unit-test suite is
currently included.

## Repository layout

- `src/`: application, camera, rays, cubes, materials, world generation, and
  SDL2 presentation.
- `benchmark/benchmark.py`: scene-matrix profiling driver.
- `benchmark/gen_scene.py`: standalone scene-file generator.
- `benchmark/scenes/`: scene fixtures.
- `benchmark/results/`: historical profiling samples and flame graphs.

## Learning references

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [Ray-tracing video reference](https://www.youtube.com/watch?v=86FAWCzIe_4)
