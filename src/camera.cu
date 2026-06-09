#include "camera.h"
#include <cuda_runtime.h>

__global__ void add(int *a, int *b, int *c) {
  int index = threadIdx.x;
  c[index] = a[index] + b[index];
}

void testCudaKernel() {
  constexpr int N = 5;
  int a[N] = {1, 2, 3, 4, 5};
  int b[N] = {10, 20, 30, 40, 50};
  int c[N] = {};

  int *d_a, *d_b, *d_c;
  cudaMalloc(&d_a, N * sizeof(int));
  cudaMalloc(&d_b, N * sizeof(int));
  cudaMalloc(&d_c, N * sizeof(int));

  cudaMemcpy(d_a, a, N * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_b, b, N * sizeof(int), cudaMemcpyHostToDevice);

  add<<<1, N>>>(d_a, d_b, d_c);
  cudaDeviceSynchronize();

  cudaMemcpy(c, d_c, N * sizeof(int), cudaMemcpyDeviceToHost);

  printf("CUDA test kernel result: ");
  for (int i = 0; i < N; i++)
    printf("%d ", c[i]);
  printf("\n");

  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
}

void renderWorld(const CameraData &host_data, const World &host_world,
                 std::array<uint32_t, window::SIZE> &host_pixels) {

  std::array<uint32_t, window::SIZE> device_pixels;

  // for (uint32_t j = 0; j < height; j++) {
  //   const auto row_start =
  //       pixel00_loc + (static_cast<float>(j) * pixel_delta_v);
  //   auto *row = pixels.data() + (j * width);
  //   auto pixel_center = row_start;
  //   for (uint32_t i = 0; i < width; i++) {
  //     color pixel_color(0, 0, 0);
  //     for (int s = 0; s < camera::SAMPLES_PER_PIXEL; s++) {
  //       const float x_offset =
  //           std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);
  //       const float y_offset =
  //           std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);
  //
  //       auto sample_point =
  //           pixel_center + x_offset * pixel_delta_u + y_offset *
  //           pixel_delta_v;
  //
  //       ray r(center, sample_point - center);
  //
  //       pixel_color += ray_color(r, world, camera::MAX_DEPTH);
  //     }
  //     pixel_color /= static_cast<float>(camera::SAMPLES_PER_PIXEL);
  //     *row++ = convert(pixel_color);
  //
  //     pixel_center += pixel_delta_u;
  //   }
  // }
}
