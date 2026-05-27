#include "face.h"

#include <cmath>

namespace {
constexpr float kEpsilon = 1e-6f;

bool hitQuad(const ray &r, const point3 &origin, const vec3 &u, const vec3 &v,
             vec3 &normal, float &t) {
  const vec3 planeNormal = glm::cross(u, v);
  const float normalLen2 = glm::dot(planeNormal, planeNormal);
  if (normalLen2 < kEpsilon * kEpsilon) {
    return false;
  }

  const float denom = glm::dot(planeNormal, r.direction());
  if (std::abs(denom) < kEpsilon) {
    return false;
  }

  t = glm::dot(planeNormal, origin - r.origin()) / denom;
  if (t <= kEpsilon) {
    return false;
  }

  const point3 p = r.origin() + t * r.direction();
  const vec3 w = p - origin;

  const float uu = glm::dot(u, u);
  const float uv = glm::dot(u, v);
  const float vv = glm::dot(v, v);
  const float wu = glm::dot(w, u);
  const float wv = glm::dot(w, v);
  const float det = uu * vv - uv * uv;
  if (std::abs(det) < kEpsilon) {
    return false;
  }

  const float s = (wu * vv - wv * uv) / det;
  const float vcoord = (wv * uu - wu * uv) / det;
  if (s < -kEpsilon || s > 1.0f + kEpsilon || vcoord < -kEpsilon ||
      vcoord > 1.0f + kEpsilon) {
    return false;
  }

  normal = glm::normalize(planeNormal);
  return true;
}
} // namespace

bool Face::hit(const ray &r, vec3 &normal, float &t) const {
  return hitQuad(r, get_pos(), u, v, normal, t);
}
