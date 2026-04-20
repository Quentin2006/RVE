#pragma once

#include "consts.h"
#include <cstdint>

__global__ void my_first_kernel(uint32_t *data) {
  uint i = (blockIdx.x * blockDim.x) + threadIdx.x;

  if (i >= WIDTH * HEIGHT) {
    return;
  }

  uint x = i % WIDTH;
  uint y = i / WIDTH;

  int r = (int)(255.999 * double(x) / (WIDTH - 1));
  int g = (int)(255.999 * double(y) / (HEIGHT - 1));
  int b = 0;

  data[i] = (uint32_t)((r << 16) | (g << 8) | b);
}
