#include "camera.h"
#include "consts.h"
#include "window.h"
#include "world.h"

#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

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
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W] != 0U) {
      camera.moveForward(-0.1f);
    }
    if (keys[SDL_SCANCODE_S] != 0U) {
      camera.moveForward(0.1f);
    }
    if (keys[SDL_SCANCODE_D] != 0U) {
      camera.moveRight(-0.1f);
    }
    if (keys[SDL_SCANCODE_A] != 0U) {
      camera.moveRight(0.1f);
    }
    if (keys[SDL_SCANCODE_SPACE] != 0U) {
      camera.moveUp(-0.1f);
    }
    if (keys[SDL_SCANCODE_LSHIFT] != 0U) {
      camera.moveUp(0.1f);
    }

    camera.render(world, pixels);

    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
