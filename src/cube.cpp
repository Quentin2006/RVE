#include "cube.h"

Cube::Cube(float x, float y, float z) : face(Quad({x, y, z}, {0, 0, 1})) {}

vec3 Cube::is_hit(const ray &r) const {
  if (face.is_hit(r)) {
    return {1, 0, 0};
  };
  return {0, 0, 0};
}
