#include "face.h"

#include <cmath>

void Face::update() {
  origin = get_pos();

  const vec3 rawNormal = glm::cross(u, v);
  const float normalLen2 = glm::dot(rawNormal, rawNormal);
  if (normalLen2 < math::K_EPSILON * math::K_EPSILON) {
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
  valid = std::abs(det) >= math::K_EPSILON;
  invDet = valid ? 1.0f / det : 0.0f;
}

bool Face::hit(const ray &r, HitRecord &rec, float t_max) const {
  if (!valid) {
    return false;
  }

  const float denom = glm::dot(faceNormal, r.direction());
  if (std::abs(denom) < math::K_EPSILON) {
    return false;
  }

  rec.t = glm::dot(faceNormal, origin - r.origin()) / denom;
  if (rec.t <= math::K_EPSILON || rec.t > t_max) {
    return false;
  }

  const vec3 w = r.origin() + rec.t * r.direction() - origin;

  const float wu = glm::dot(w, u);
  const float wv = glm::dot(w, v);
  const float s = (wu * vv - wv * uv) * invDet;
  const float vcoord = (wv * uu - wu * uv) * invDet;
  if (s < -math::K_EPSILON || s > 1.0f + math::K_EPSILON ||
      vcoord < -math::K_EPSILON || vcoord > 1.0f + math::K_EPSILON) {
    return false;
  }

  rec.normal = faceNormal;
  rec.color = get_color();
  return true;
}
