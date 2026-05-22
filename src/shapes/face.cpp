#include "face.h"

bool Face::is_hit(const ray &r, vec3 &normal, float &t) const {
  point3 p = get_pos();

  point3 A = p;
  point3 B = p + u;
  point3 C = p + v;
  point3 D = p + u + v;

  Triangle t1(A, B, C);
  Triangle t2(D, C, B);

  return t1.is_hit(r, normal, t) || t2.is_hit(r, normal, t);
}
