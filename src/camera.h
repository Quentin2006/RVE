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

  void render(const World &world,
              std::array<uint32_t, window::SIZE> &pixels) const;

private:
  color ray_color(const ray &r, const World &world) const;
  uint32_t width;
  uint32_t height;
  point3 center;
  point3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
};
