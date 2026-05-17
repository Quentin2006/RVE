#include "cube.h"

Cube::Cube(const point3 &p)
    : faces{
          Face(p + vec3{0, 0, 1}, {1, 0, 0}, {0, 1, 0}),  // front
          Face(p + vec3{0, 0, -1}, {1, 0, 0}, {0, 1, 0}), // back
          Face(p + vec3{0, 1, 0}, {1, 0, 0}, {0, 0, 1}),  // top
          Face(p + vec3{0, -1, 0}, {1, 0, 0}, {0, 0, 1}), // bottom
          Face(p + vec3{1, 0, 0}, {0, 1, 0}, {0, 0, 1}),  // right
          Face(p + vec3{-1, 0, 0}, {0, 1, 0}, {0, 0, 1})  // left
      } {}
bool Cube::is_hit(const ray &r, vec3 &normal) const {

  for (const auto &face : faces) {
    if (face.is_hit(r, normal)) {
      return true;
    }
  }
  return false;
}

void Cube::update(void) {
  point3 p = get_pos();

  for (Face &face : faces) {
    face.move(p);
  }
}
