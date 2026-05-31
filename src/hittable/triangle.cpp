#include "triangle.h"
#include <cmath>

bool Triangle::hit(const ray &r, float ray_min, float ray_max,
                   HitRecord &rec) const {
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

  const float t = glm::dot(AC, qvec) * invDet;
  if (t < ray_min || t >= ray_max) {
    return false;
  }

  rec.t = t;
  rec.normal = triangleNormal;
  rec.point = r.origin() + t * r.direction();

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
