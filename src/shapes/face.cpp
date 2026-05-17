#include "face.h"

Face::Face(point3 p, vec3 u, vec3 v) : A({0, 0, 0}), B(u), C(v), D(u + v) {
  move(p); // or move(p)
  update();
}

bool Face::is_hit(const ray &r, vec3 &normal) const {
  return top.is_hit(r, normal) || bottom.is_hit(r, normal);
}

void Face::update() {
  point3 p = get_pos();
  top = Triangle(p, A, B, C);
  bottom = Triangle(p, D, B, C);
}
