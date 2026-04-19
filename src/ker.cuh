#include "color.h"
#include "consts.h"
#include <cstdint>

__global__ void my_first_kernel(uint32_t *data) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;

  if (i >= WIDTH * HEIGHT)
    return;

  int x = i % WIDTH;
  int y = i / WIDTH;

  auto r = (uint8_t)(255.999 * double(x) / (WIDTH - 1));
  auto g = (uint8_t)(255.999 * double(y) / (HEIGHT - 1));
  auto b = (uint8_t)0;

  data[i] = (r << 16) | (g << 8) | b;

  data[i] = RGB_TO_HEX(r, g, b);
}
