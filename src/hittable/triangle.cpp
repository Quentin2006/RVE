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

bool Triangle::hit(const ray &r, vec3 &normal, float &t) const {
  if (!valid) {
    return false;
  }

  const vec3 pvec = glm::cross(r.direction(), AC);
  const float det = glm::dot(AB, pvec);
  if (std::abs(det) < kEpsilon) {
    return false;
  }

  const float invDet = 1.0f / det;
  const vec3 tvec = r.origin() - A;
  const float u = glm::dot(tvec, pvec) * invDet;
  if (u < -kEpsilon || u > 1.0f + kEpsilon) {
    return false;
  }

  const vec3 qvec = glm::cross(tvec, AB);
  const float v = glm::dot(r.direction(), qvec) * invDet;
  if (v < -kEpsilon || u + v > 1.0f + kEpsilon) {
    return false;
  }

  t = glm::dot(AC, qvec) * invDet;
  if (t <= kEpsilon) {
    return false;
  }

  normal = triangleNormal;
  return true;
}

void Triangle::update(void) {
  AB = B - A;
  AC = C - A;

  const vec3 rawNormal = glm::cross(AB, AC);
  const float normalLen2 = glm::dot(rawNormal, rawNormal);
  if (normalLen2 < kEpsilon * kEpsilon) {
    triangleNormal = vec3(0.0f);
    valid = false;
    return;
  }

  triangleNormal = rawNormal / std::sqrt(normalLen2);
  valid = true;
}
