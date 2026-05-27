
#include "world.h"

bool World::hit(const ray &r, vec3 &normal, float &t, float t_max) const {
  bool hit_anything = false;
  float closest_t = t_max;
  vec3 closest_normal{0.0f};
  float closest_hit_t = t_max;

  for (const auto &object : world) {
    vec3 candidate_normal;
    float candidate_t;
    if (object->hit(r, candidate_normal, candidate_t, closest_t) &&
        candidate_t < closest_t) {
      hit_anything = true;
      closest_t = candidate_t;
      closest_normal = candidate_normal;
      closest_hit_t = candidate_t;
    }
  }

  if (hit_anything) {
    normal = closest_normal;
    t = closest_hit_t;
  }

  return hit_anything;
}
