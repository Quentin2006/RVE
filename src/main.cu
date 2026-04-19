#include "Window.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cuda_runtime.h>
#include <stdexcept>
#include <vector>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

// ─── CUDA ────────────────────────────────────────────────────────────────────

__global__ void renderKernel(uint32_t *pixels, int width, int height,
                             float time) {
  int x = static_cast<int>(blockIdx.x * blockDim.x + threadIdx.x);
  int y = static_cast<int>(blockIdx.y * blockDim.y + threadIdx.y);

  if (x >= width || y >= height)
    return;

  float u = static_cast<float>(x) / static_cast<float>(width);
  float v = static_cast<float>(y) / static_cast<float>(height);

  // animated gradient — replace this with your ray tracing logic
  auto r = static_cast<uint8_t>(255.0F * u);
  auto g = static_cast<uint8_t>(255.0F * v);
  auto b = static_cast<uint8_t>(255.0F * (0.5F + 0.5F * sinf(time)));

  pixels[y * width + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
}

// ─── RAII CUDA buffer
// ─────────────────────────────────────────────────────────

class CudaBuffer {
public:
  explicit CudaBuffer(size_t count) : m_count(count) {
    if (cudaMalloc(&m_ptr, count * sizeof(uint32_t)) != cudaSuccess)
      throw std::runtime_error("cudaMalloc failed");
  }

  ~CudaBuffer() { cudaFree(m_ptr); }

  CudaBuffer(const CudaBuffer &) = delete;
  CudaBuffer &operator=(const CudaBuffer &) = delete;
  CudaBuffer(CudaBuffer &&) = delete;
  CudaBuffer &operator=(CudaBuffer &&) = delete;

  [[nodiscard]] uint32_t *get() const { return m_ptr; }
  [[nodiscard]] size_t count() const { return m_count; }

  void toHost(void *dst) const {
    cudaMemcpy(dst, m_ptr, m_count * sizeof(uint32_t), cudaMemcpyDeviceToHost);
  }

private:
  uint32_t *m_ptr = nullptr;
  size_t m_count = 0;
};

// ─── main
// ─────────────────────────────────────────────────────────────────────

int main() {
  Window window{WIDTH, HEIGHT};
  CudaBuffer devicePixels(static_cast<size_t>(WIDTH * HEIGHT));
  std::vector<uint32_t> hostPixels(static_cast<size_t>(WIDTH * HEIGHT));

  // 16x16 thread blocks — standard for 2D image kernels
  dim3 blockSize(16, 16);
  dim3 gridSize((WIDTH + blockSize.x - 1) / blockSize.x,
                (HEIGHT + blockSize.y - 1) / blockSize.y);

  float time = 0.0F;

  while (!window.should_close()) {
    renderKernel<<<gridSize, blockSize>>>(devicePixels.get(), WIDTH, HEIGHT,
                                          time);
    cudaDeviceSynchronize();

    window.present(hostPixels.data());

    devicePixels.toHost(hostPixels.data());
    time += 0.01F;
  }

  return 0;
}
