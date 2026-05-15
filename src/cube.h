#pragma once

#include "quad.h"
#include "ray.h"
#include <array>
#include <glm/glm.hpp>

class Cube {
public:
  Cube(float x, float y, float z);

  vec3 is_hit(const ray &r) const;
  // void move(vec3 delta) { center += delta; }

private:
  Quad face;
};
