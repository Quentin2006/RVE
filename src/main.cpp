#include "camera.h"
#include "consts.h"
#include "hittable/cube.h"
#include "window.h"
#include "world.h"

#include <array>
#include <memory>

int main() {

  World world;

  world.add(std::make_unique<Cube>(point3{0, 0, -5}));
  Camera camera(window::WIDTH, window::HEIGHT);
  Window window{window::WIDTH, window::HEIGHT};

  if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0) {
    SDL_Log("SDL_SetRelativeMouseMode failed: %s", SDL_GetError());
  }

  int mouseDx = 0;
  int mouseDy = 0;
  SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

  constexpr float mouseSensitivity = 0.0025f;

  std::array<uint32_t, window::SIZE> pixels;
  while (!window.should_close()) {
    SDL_PumpEvents();
    SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

    camera.rotateYaw(static_cast<float>(mouseDx) * mouseSensitivity);
    camera.rotatePitch(static_cast<float>(mouseDy) * mouseSensitivity);

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
