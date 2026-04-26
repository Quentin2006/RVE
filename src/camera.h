#pragma once

#include "color.h"
#include "consts.h"
#include "ray.h"
#include "world.h"
#include <array>
#include <cstdint>

class Camera {
public:
  Camera(uint32_t _width, uint32_t _height);

  void moveForward(float distance);
  void moveRight(float distance);
  void moveUp(float distance);

  void render(const World &world,
              std::array<uint32_t, window::SIZE> &pixels) const;

private:
  color ray_color(const ray &r, const World &world) const;
  uint32_t width;
  uint32_t height;
  point3 center;
};
