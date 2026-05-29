
#include "world.h"

bool World::hit(const ray &r, HitRecord &rec, float t_max) const {
  bool hit_anything = false;
  float closest_t = t_max;

  for (const auto &object : world) {
    if (object == nullptr) {
      continue;
    }

    HitRecord candidate{};
    if (object->hit(r, candidate, closest_t) && candidate.t < closest_t) {
      hit_anything = true;
      closest_t = candidate.t;
      rec = candidate;
    }
  }

  return hit_anything;
}
