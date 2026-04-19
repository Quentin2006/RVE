#include "Window.cuh"
#include "consts.h"
#include "ker.cuh"
#include <array>
#include <glm/glm.hpp>
#include <limits>

int main() {

  Window window{WIDTH, HEIGHT};

  std::array<uint32_t, SIZE> h_pixels;
  h_pixels.fill(std::numeric_limits<uint32_t>::max());

  // Allocate GPU memory
  uint32_t *d_pixels;
  cudaMalloc(&d_pixels, SIZE * sizeof(uint32_t));

  // Copy ONCE at the start
  cudaMemcpy(d_pixels, h_pixels.data(), SIZE * sizeof(uint32_t),
             cudaMemcpyHostToDevice);

  while (!window.should_close()) {

    // Run kernel directly on GPU memory
    my_first_kernel<<<NUM_BLOCKS, BLOCK_SIZE>>>(d_pixels);

    // Copy result back for display
    cudaMemcpy(h_pixels.data(), d_pixels, SIZE * sizeof(uint32_t),
               cudaMemcpyDeviceToHost);

    window.present(h_pixels.data());
  }

  cudaFree(d_pixels);
  return 0;
}
