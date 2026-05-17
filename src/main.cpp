#include "camera.h"
#include "consts.h"
#include "shapes/cube.h"
#include "window.h"
#include "world.h"

#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

int main() {

  World world;

  Triangle tri({0, 0, 0}, {0, 1, 0}, {1, 0, 0});
  Face face({0, -1, 0}, {1, 0, 0}, {0, 1, 0});
  Cube cube({1, 0, 0});

  std::vector<std::unique_ptr<Hittable>> objs;
  objs.push_back(std::make_unique<Triangle>(tri));
  objs.push_back(std::make_unique<Face>(face));
  objs.push_back(std::make_unique<Cube>(cube));

  // world.add(std::move(objs[0]));
  // world.add(std::move(objs[1]));
  world.add(std::move(objs[2]));

  Camera camera(window::WIDTH, window::HEIGHT);

  Window window{window::WIDTH, window::HEIGHT};

  std::array<uint32_t, window::SIZE> pixels;
  while (!window.should_close()) {
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W] != 0U) {
      camera.moveForward(0.1f);
    }
    if (keys[SDL_SCANCODE_S] != 0U) {
      camera.moveForward(-0.1f);
    }
    if (keys[SDL_SCANCODE_D] != 0U) {
      camera.moveRight(0.1f);
    }
    if (keys[SDL_SCANCODE_A] != 0U) {
      camera.moveRight(-0.1f);
    }
    if (keys[SDL_SCANCODE_SPACE] != 0U) {
      camera.moveUp(0.1f);
    }
    if (keys[SDL_SCANCODE_LSHIFT] != 0U) {
      camera.moveUp(-0.1f);
    }

    camera.render(world, pixels);

    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
