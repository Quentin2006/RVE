
# RVE(Ray-traced Voxel Engine)

A voxel-based ray-tracing engine built from scratch in Rust

---

## Stack

| Layer | Tool |
|---|---|
| Language | Rust|
| GPU Abstraction | `wgpu` |
| Shaders | WGSL  |
| Windowing / Input | `winit` |
| Debug UI | `egui` |

---

## Progress

### Phase 1 — Foundation

- [ ] wgpu device + surface setup
- [ ] `winit` event loop with resize handling
- [ ] Fullscreen quad render pass
- [ ] Basic compute shader writing solid color to output texture
- [ ] Camera uniform buffer + ray generation in shader

### Phase 2 — Voxel Traversal

- [ ] Flat voxel grid in GPU storage buffer
- [ ] DDA traversal in WGSL — first hit detection
- [ ] Normal-based shading (color by face direction)
- [ ] Diffuse lighting with a single directional light

### Phase 3 — World Structure

- [ ] Chunk system (32x32x32 per chunk)
- [ ] CPU-side chunk registry with dirty flagging
- [ ] Procedural world gen (CPU) → upload to GPU
- [ ] Skip empty chunks during traversal

### Phase 4 — Optimization

- [ ] BVH over chunks (skip empty regions fast)
- [ ] Sparse Voxel Octree (SVO) — replace flat grid
- [ ] Shader hot-reload (`naga` / file watcher)
- [ ] Frame timing + GPU profiling overlay (egui)

### Phase 5 — Visual Features

- [ ] Shadow rays (secondary ray per hit)
- [ ] Ambient occlusion (cone sampling)
- [ ] Reflective materials
- [ ] Transparency + refraction
- [ ] Sky / atmosphere shader

### Phase 6 — Stretch Goals

- [ ] Global illumination (path tracing mode)
- [ ] Dynamic voxel editing at runtime
- [ ] WebGPU target (WASM build)
- [ ] Chunk streaming from disk

---

## Build & Run

```bash
# Debug build (fast compile, slow runtime)
cargo run

# Release build (optimized — use for actual rendering)
cargo run --release

# Run with validation layers enabled (catches GPU errors)
WGPU_BACKEND=vulkan cargo run
```

**Requirements:**

- Rust 1.75+
- A GPU with Vulkan, Metal, or DX12 support
- `wgpu`, `winit`, `egui` — all pulled via `Cargo.toml`

---
