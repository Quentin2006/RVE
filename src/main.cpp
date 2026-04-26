#include "camera.h"
#include "consts.h"
#include "window.h"
#include "world.h"

#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <vector>

std::vector<Cube> scene;

int main() {

  World world;
  world.add(std::make_unique<Cube>(0, 0, -5));
  world.add(std::make_unique<Cube>(0, 1, -5));
  world.add(std::make_unique<Cube>(0, 2, -5));
  world.add(std::make_unique<Cube>(1, 1, -5));

  Camera camera(window::WIDTH, window::HEIGHT);

  Window window{window::WIDTH, window::HEIGHT};

  std::array<uint32_t, window::SIZE> pixels;

  while (!window.should_close()) {
    camera.render(world, pixels);
    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
