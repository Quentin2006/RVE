#include "camera.h"
#include "consts.h"
#include "hittable/cube.h"
#include "window.h"
#include "world.h"

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

int main() {

  World world;
  std::vector<uint> ids;

  for (int x = 0; x < 1; ++x) {
    for (int y = 0; y < 4; ++y) {
      int id = world.add(std::make_unique<Cube>(point3{x, y, -5}));
      ids.push_back(id);
    }
  }

  Camera camera(window::WIDTH, window::HEIGHT);
  Window window{window::WIDTH, window::HEIGHT};

  if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0) {
    SDL_Log("SDL_SetRelativeMouseMode failed: %s", SDL_GetError());
  }

  int mouseDx = 0;
  int mouseDy = 0;
  SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

  constexpr float mouseSensitivity = 0.25f;
  constexpr float moveStep = 1.f;

  std::array<uint32_t, window::SIZE> pixels;

  float delta_time = 0;
  float elapsed_time = 0;

  while (!window.should_close()) {
    window.processEvents();

    SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

    camera.rotateYaw(static_cast<float>(mouseDx) * mouseSensitivity *
                     delta_time);
    camera.rotatePitch(static_cast<float>(mouseDy) * mouseSensitivity *
                       delta_time);

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W] != 0U) {
      camera.moveForward(moveStep * delta_time);
    }
    if (keys[SDL_SCANCODE_S] != 0U) {
      camera.moveForward(-moveStep * delta_time);
    }
    if (keys[SDL_SCANCODE_D] != 0U) {
      camera.moveRight(moveStep * delta_time);
    }
    if (keys[SDL_SCANCODE_A] != 0U) {
      camera.moveRight(-moveStep * delta_time);
    }
    if (keys[SDL_SCANCODE_SPACE] != 0U) {
      camera.moveUp(moveStep * delta_time);
    }
    if (keys[SDL_SCANCODE_LSHIFT] != 0U) {
      camera.moveUp(-moveStep * delta_time);
    }

    const auto start{std::chrono::steady_clock::now()};

    camera.render(world, pixels);

    window.present(pixels);

    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    delta_time = elapsed_seconds.count();

    elapsed_time += delta_time;

    const float fps = 1.f / elapsed_seconds.count();

    std::cerr << "FPS: " << fps << '\n';

    if ((int)elapsed_time % 10 == 5) {
      world.remove(0);
      world.remove(1);
      world.remove(2);
    }
  }

  return EXIT_SUCCESS;
}
