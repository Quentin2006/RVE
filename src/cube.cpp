#include "cube.h"
#include "ray.h"

Cube::Cube(const point3 &point)
    : faces{
          Quad(point + point3(0.0, 0.0, 0.5), {0, 0, 1}),   // Front (+Z)
          Quad(point + point3(0.0, 0.0, -0.5), {0, 0, -1}), // Back  (-Z)
          Quad(point + point3(-0.5, 0.0, 0.0), {-1, 0, 0}), // Left  (-X)
          Quad(point + point3(0.5, 0.0, 0.0), {1, 0, 0}),   // Right (+X)
          Quad(point + point3(0.0, 0.5, 0.0), {0, 1, 0}),   // Top    (+Y)
          Quad(point + point3(0.0, -0.5, 0.0), {0, -1, 0})  // Bottom (-Y)
      } {}

vec3 Cube::is_hit(const ray &r) const {

  for (const auto &face : faces) {
    vec3 norm = face.is_hit(r);
    if (norm != vec3{0, 0, 0}) {
      return norm;
    };
  }
  return {0, 0, 0};
}
