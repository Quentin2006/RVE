#include "triangle.h"
#include <cmath>

namespace {
constexpr float kEpsilon = 1e-6f;
}

Triangle::Triangle(Material material)
    : Hittable(material), A({0, 0, 0}), B({0, 0, 0}), C({0, 0, 0}) {
  move({0, 0, 0});
}

Triangle::Triangle(Material material, point3 A_, point3 B_, point3 C_)
    : Hittable(material), A(A_), B(B_), C(C_) {
  move({0, 0, 0});
}

bool Triangle::is_hit(const ray &r, vec3 &normal, float &t) const {
  if (glm::dot(N, N) < kEpsilon * kEpsilon) {
    return false;
  }

  const float denom = glm::dot(N, r.direction());
  if (std::abs(denom) < kEpsilon) {
    return false;
  }

  const float D = -glm::dot(N, A);

  t = -(glm::dot(N, r.origin()) + D) / denom;

  if (t <= kEpsilon) {
    return false;
  }

  const vec3 P = r.origin() + t * r.direction();

  vec3 Ne;
  const vec3 Ap = P - A;
  Ne = glm::cross(AB, Ap);
  if (glm::dot(N, Ne) < -kEpsilon) {
    return false;
  }

  const vec3 Bp = P - B;
  Ne = glm::cross(BC, Bp);
  if (glm::dot(N, Ne) < -kEpsilon) {
    return false;
  }

  const vec3 Cp = P - C;
  Ne = glm::cross(CA, Cp);
  if (glm::dot(N, Ne) < -kEpsilon) {
    return false;
  }

  normal = N;
  return true;
}

bool Triangle::hit(const ray &r, vec3 &normal, float &t) const {
  return is_hit(r, normal, t);
}

void Triangle::update(void) {
  AB = B - A;
  BC = C - B;
  CA = A - C;

  const vec3 rawNormal = glm::cross(AB, C - A);
  const float normalLen2 = glm::dot(rawNormal, rawNormal);
  if (normalLen2 < kEpsilon * kEpsilon) {
    N = vec3(0.0f);
    return;
  }

  N = rawNormal / std::sqrt(normalLen2);
}
