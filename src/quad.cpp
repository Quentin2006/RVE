#include "quad.h"
#include <cmath>
#include <glm/geometric.hpp>

Quad::Quad(vec3 point, vec3 normal) : normal(normal), point(point) {
  if (dot(normal, normal) < 1e-6f) {
    u = {1, 0, 0};
    v = {0, 1, 0};
    return;
  }

  vec3 helper = (std::fabs(normal.x) > std::fabs(normal.z)) ? vec3(0, 0, 1)
                                                            : vec3(1, 0, 0);
  u = normalize(cross(helper, normal));
  v = normalize(cross(normal, u));
}

bool Quad::is_hit(const ray &r) const {
  float denom = dot(normal, r.direction());

  if (fabs(denom) < 1e-6)
    return false;

  float t = dot(point - r.origin(), normal) / denom;

  if (t < 0)
    return false;

  vec3 p = r.origin() + t * r.direction();
  vec3 rel = p - point;

  float uu = dot(u, u);
  float uv = dot(u, v);
  float vv = dot(v, v);

  float ru = dot(rel, u);
  float rv = dot(rel, v);

  float denom2 = uu * vv - uv * uv;
  if (std::fabs(denom2) < 1e-6f)
    return false;

  float a = (ru * vv - rv * uv) / denom2;
  float b = (rv * uu - ru * uv) / denom2;

  return (a >= 0 && a <= 1 && b >= 0 && b <= 1);
}
