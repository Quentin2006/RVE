#include "world.h"

bool World::hit(const ray &r, vec3 &normal, float &t, float t_max) const {
  bool hit_anything = false;
  float closest_t = t_max;

  for (const auto &object : world) {
    if (object == nullptr) {
      continue;
    }

    vec3 candidate_normal;
    float candidate_t;
    if (object->hit(r, candidate_normal, candidate_t, closest_t) &&
        candidate_t < closest_t) {
      hit_anything = true;
      closest_t = candidate_t;
      normal = candidate_normal;
      t = candidate_t;
    }
  }

  return hit_anything;
}
