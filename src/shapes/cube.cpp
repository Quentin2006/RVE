#include "cube.h"
#include <array>
#include <cmath>
#include <utility>

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

bool Cube::is_hit(const ray &r, vec3 &normal, float &t) const {

  float nearest_t = INFINITY;
  vec3 nearest_n = {0, 0, 0};

  for (const auto &face : faces) {
    float temp_t;
    vec3 temp_n;

    if (face.is_hit(r, temp_n, temp_t) && temp_t < nearest_t) {
      nearest_t = temp_t;
      nearest_n = temp_n;
    }
  }

  if (nearest_t != INFINITY) {
    normal = nearest_n;
    t = nearest_t;
  }
  return nearest_t != INFINITY;
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
