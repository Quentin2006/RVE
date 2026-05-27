#include "face.h"

#include <cmath>

namespace {
constexpr float kEpsilon = 1e-6f;
}

void Face::update() {
  origin = get_pos();

  const vec3 rawNormal = glm::cross(u, v);
  const float normalLen2 = glm::dot(rawNormal, rawNormal);
  if (normalLen2 < kEpsilon * kEpsilon) {
    faceNormal = vec3(0.0f);
    uu = 0.0f;
    uv = 0.0f;
    vv = 0.0f;
    invDet = 0.0f;
    valid = false;
    return;
  }

  faceNormal = rawNormal / std::sqrt(normalLen2);
  uu = glm::dot(u, u);
  uv = glm::dot(u, v);
  vv = glm::dot(v, v);

  const float det = uu * vv - uv * uv;
  valid = std::abs(det) >= kEpsilon;
  invDet = valid ? 1.0f / det : 0.0f;
}

bool Face::hit(const ray &r, vec3 &normal, float &t) const {
  if (!valid) {
    return false;
  }

  const float denom = glm::dot(faceNormal, r.direction());
  if (std::abs(denom) < kEpsilon) {
    return false;
  }

  t = glm::dot(faceNormal, origin - r.origin()) / denom;
  if (t <= kEpsilon) {
    return false;
  }

  const vec3 w = r.origin() + t * r.direction() - origin;

  const float wu = glm::dot(w, u);
  const float wv = glm::dot(w, v);
  const float s = (wu * vv - wv * uv) * invDet;
  const float vcoord = (wv * uu - wu * uv) * invDet;
  if (s < -kEpsilon || s > 1.0f + kEpsilon || vcoord < -kEpsilon ||
      vcoord > 1.0f + kEpsilon) {
    return false;
  }

  normal = faceNormal;
  return true;
}
