#pragma once

#include "quad.h"
#include "ray.h"
#include <array>
#include <glm/glm.hpp>

class Cube {
public:
  Cube(const point3 &point);

  vec3 is_hit(const ray &r) const;
  // void move(vec3 delta) { center += delta; }

private:
  std::array<Quad, 6> faces;
};
