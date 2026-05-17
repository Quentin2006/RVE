#include "cube.h"

Cube::Cube(const point3 &p)
    : faces{
          Face(point3(p.x, p.y, p.z + 1), {1, 0, 0}, {0, 1, 0}), // front (+Z)
          Face(point3(p.x, p.y, p.z), {0, 1, 0}, {1, 0, 0}),     // back (-Z)

          Face(point3(p.x, p.y + 1, p.z), {0, 0, 1}, {1, 0, 0}), // top (+Y)
          Face(point3(p.x, p.y, p.z), {1, 0, 0}, {0, 0, 1}),     // bottom (-Y)
                                                                 //
          Face(point3(p.x + 1, p.y, p.z), {0, 0, 1}, {0, 1, 0}), // right (+X)
          Face(point3(p.x, p.y, p.z), {0, 1, 0}, {0, 0, 1})      // left (-X)
      } {
  move(p);
}

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

  faces[0].move(p + point3(0, 0, 1));
  faces[1].move(p + point3(0, 0, 0));
  faces[2].move(p + point3(0, 1, 0));
  faces[3].move(p + point3(0, 0, 0));
  faces[4].move(p + point3(1, 0, 0));
  faces[5].move(p + point3(0, 0, 0));
}
