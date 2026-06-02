#include "world.h"

bool World::hit(const ray &r, float ray_min, float ray_max,
                HitRecord &rec) const {
  bool hit_anything = false;
  float closest_t = ray_max;

  for (const auto &object : world) {
    HitRecord candidate_rec;
    if (object.hit(r, ray_min, closest_t, candidate_rec) &&
        candidate_rec.t < closest_t) {
      hit_anything = true;
      closest_t = candidate_rec.t;
      rec = candidate_rec;
    }
  }

  return hit_anything;
}
