#include "triangle.h"

Triangle::Triangle() : A({0, 0, 0}), B({0, 0, 0}), C({0, 0, 0}) { update(); }

Triangle::Triangle(point3 A_, point3 B_, point3 C_) : A(A_), B(B_), C(C_) {
  update();
}

// PERF: change to Möller-Trumbore algorithm in the future
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html
bool Triangle::is_hit(const ray &r, vec3 &normal, float &t) const {

  // distance from orgin
  float D = -dot(N, A);

  t = -(dot(N, r.origin()) + D) / glm::dot(N, r.direction());

  // ray is behind tri
  if (t < 0) {
    return false;
  }

  vec3 P = r.origin() + t * r.direction();

  // Step 2: Inside-Outside Test
  vec3 Ne; // Vector perpendicular to triangle's plane
  vec3 Ap = P - A;
  Ne = cross(AB, Ap);
  if (dot(N, Ne) < 0)
    return false;

  vec3 Bp = P - B;
  Ne = cross(BC, Bp);
  if (dot(N, Ne) < 0)
    return false;

  vec3 Cp = P - C;
  Ne = cross(CA, Cp);
  if (dot(N, Ne) < 0)
    return false;

  normal = N;
  return true;
}

void Triangle::update(void) {
  AB = B - A;
  BC = C - B;
  CA = A - C;

  N = normalize(cross(AB, C - A));
}
