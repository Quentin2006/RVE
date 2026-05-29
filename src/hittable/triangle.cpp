#include "triangle.h"
#include <cmath>

Triangle::Triangle(Material material, ::color object_color)
    : Hittable(material, object_color), A({0, 0, 0}), B({0, 0, 0}), C({0, 0, 0}) {
  move({0, 0, 0});
}

Triangle::Triangle(Material material, point3 A_, point3 B_, point3 C_,
                   ::color object_color)
    : Hittable(material, object_color), A(A_), B(B_), C(C_) {
  move({0, 0, 0});
}

bool Triangle::hit(const ray &r, HitRecord &rec, float t_max) const {
  if (!valid) {
    return false;
  }

  const vec3 pvec = glm::cross(r.direction(), AC);
  const float det = glm::dot(AB, pvec);
  if (std::abs(det) < math::K_EPSILON) {
    return false;
  }

  const float invDet = 1.0f / det;
  const vec3 tvec = r.origin() - A;
  const float u = glm::dot(tvec, pvec) * invDet;
  if (u < -math::K_EPSILON || u > 1.0f + math::K_EPSILON) {
    return false;
  }

  const vec3 qvec = glm::cross(tvec, AB);
  const float v = glm::dot(r.direction(), qvec) * invDet;
  if (v < -math::K_EPSILON || u + v > 1.0f + math::K_EPSILON) {
    return false;
  }

  rec.t = glm::dot(AC, qvec) * invDet;
  if (rec.t <= math::K_EPSILON || rec.t > t_max) {
    return false;
  }

  rec.normal = triangleNormal;
  rec.color = get_color();
  return true;
}

void Triangle::update(void) {
  AB = B - A;
  AC = C - A;

  const vec3 rawNormal = glm::cross(AB, AC);
  const float normalLen2 = glm::dot(rawNormal, rawNormal);
  if (normalLen2 < math::K_EPSILON * math::K_EPSILON) {
    triangleNormal = vec3(0.0f);
    valid = false;
    return;
  }

  triangleNormal = rawNormal / std::sqrt(normalLen2);
  valid = true;
}
