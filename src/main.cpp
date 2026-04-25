#include "color.h"
#include "consts.h"
#include "window.h"

#include <array>
#include <glm/glm.hpp>
#include <iostream>

int main() {

  Window window{window::WIDTH, window::HEIGHT};

  std::array<uint32_t, window::SIZE> pixels;

  while (!window.should_close()) {

    for (int j = 0; j < window::HEIGHT; j++) {
      std::clog << "\rScanlines remaining: " << (window::HEIGHT - j) << ' '
                << std::flush;
      for (int i = 0; i < window::WIDTH; i++) {
        color pixel_color = color(double(i) / (window::WIDTH - 1),
                                  double(j) / (window::HEIGHT - 1), 0);

        pixels[j * window::WIDTH + i] = pixel_color.convert();
      }
    }

    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
