#include "app.h"
#include <chrono>
#include <glm/gtc/random.hpp>
#include <iostream>

void App::run() {
  // int matte = world.add(Cube(VoxelData{.mat = MaterialType::LAMBERTIAN,
  //                                      .albedo = color{1, 1, 1},
  //                                      .pos = vec3{-1, 0, -4}}));
  //
  // int metal = world.add(Cube(VoxelData{.mat = MaterialType::METAL,
  //                                      .albedo = color{.5f, 1.f, .5f},
  //                                      .fuzz = 0.f,
  //                                      .pos = vec3{0, 1, -4}}));
  //
  // int glass = world.add(Cube(VoxelData{.mat = MaterialType::DIELECTRIC,
  //                                      .albedo = color{.5f, .5f, 1.f},
  //                                      .refraction_index = 1.5f,
  //                                      .pos = vec3{1, 0, -4}}));
  //
  // int light = world.add(Cube(VoxelData{.mat = MaterialType::EMISSIVE,
  //                                      .albedo = color{1, 0, 0},
  //                                      .brightness = 100.f,
  //                                      .pos = vec3{0, -1, -4}}));

  if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0) {
    SDL_Log("SDL_SetRelativeMouseMode failed: %s", SDL_GetError());
  }

  int mouseDx = 0;
  int mouseDy = 0;
  SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

  std::array<uint32_t, window::SIZE> pixels;

  float delta_time = 0;
  float fps_elapsed_time = 0;
  uint32_t fps_frame_count = 0;

  while (!window.should_close()) {
    window.processEvents();

    SDL_GetRelativeMouseState(&mouseDx, &mouseDy);

    camera.rotateYaw(static_cast<float>(mouseDx) * input::LOOK_SPEED *
                     delta_time);
    camera.rotatePitch(static_cast<float>(mouseDy) * input::LOOK_SPEED *
                       delta_time);

    const Uint8 *keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W] != 0U) {
      camera.moveForward(input::MOVE_SPEED * delta_time);
    }
    if (keys[SDL_SCANCODE_S] != 0U) {
      camera.moveForward(-input::MOVE_SPEED * delta_time);
    }
    if (keys[SDL_SCANCODE_D] != 0U) {
      camera.moveRight(input::MOVE_SPEED * delta_time);
    }
    if (keys[SDL_SCANCODE_A] != 0U) {
      camera.moveRight(-input::MOVE_SPEED * delta_time);
    }
    if (keys[SDL_SCANCODE_SPACE] != 0U) {
      camera.moveUp(input::MOVE_SPEED * delta_time);
    }
    if (keys[SDL_SCANCODE_LSHIFT] != 0U) {
      camera.moveUp(-input::MOVE_SPEED * delta_time);
    }

    const auto start{std::chrono::steady_clock::now()};

    camera.render(world, pixels);

    window.present(pixels);

    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    delta_time = elapsed_seconds.count();

    fps_elapsed_time += delta_time;
    ++fps_frame_count;

    if (fps_elapsed_time >= 1.f) {
      const float avg_fps =
          static_cast<float>(fps_frame_count) / fps_elapsed_time;
      std::cerr << "FPS: " << avg_fps << '\n';
      fps_elapsed_time = 0.f;
      fps_frame_count = 0;
    }
  }
}
