#include "color.h"
#include "consts.h"
#include "window.h"

#include <array>
#include <glm/glm.hpp>

int main() {

  Window window{window::WIDTH, window::HEIGHT};

  std::array<uint32_t, window::SIZE> pixels;

  color white(1.f, 1.f, 1.f);

  pixels.fill(white.convert());

  while (!window.should_close()) {
    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
