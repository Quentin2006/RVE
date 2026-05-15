#include "quad.h"
#include <cmath>
#include <glm/geometric.hpp>

Quad::Quad(vec3 point_, vec3 normal_) : normal(normal_), point(point_) {
  if (dot(normal_, normal_) < 1e-6f) {
    u = {1, 0, 0};
    v = {0, 1, 0};
    return;
  }

  vec3 helper = (std::fabs(normal_.x) > std::fabs(normal_.z)) ? vec3(0, 0, 1)
                                                              : vec3(1, 0, 0);
  u = normalize(cross(helper, normal_));
  v = normalize(cross(normal_, u));
}

vec3 Quad::is_hit(const ray &r) const {
  float denom = dot(normal, r.direction());

  if (fabs(denom) < 1e-6)
    return {0, 0, 0};

  float t = dot(point - r.origin(), normal) / denom;

  if (t < 0)
    return {0, 0, 0};

  vec3 p = r.origin() + t * r.direction();
  vec3 rel = p - point;

  float uu = dot(u, u);
  float uv = dot(u, v);
  float vv = dot(v, v);

  float ru = dot(rel, u);
  float rv = dot(rel, v);

  float denom2 = uu * vv - uv * uv;
  if (std::fabs(denom2) < 1e-6f)
    return {0, 0, 0};

  float a = (ru * vv - rv * uv) / denom2;
  float b = (rv * uu - ru * uv) / denom2;

  bool hit = (a >= 0 && a <= 1 && b >= 0 && b <= 1);

  if (hit) {
    return normal;
  }
  return {0, 0, 0};
}
