#pragma once

#include "../color.h"
#include "../consts.h"
#include "../ray.h"

#include <glm/gtc/random.hpp>
inline vec3 random_unit_vector() {
  glm::vec3 random_direction = glm::sphericalRand(1.0f);
  glm::vec3 random_unit_vector = glm::normalize(random_direction);

  return random_unit_vector;
}

enum MaterialType { LAMBERTIAN, METAL, DIELECTRIC, EMMISSIVE, NONE };

class Hittable;

struct HitRecord {
  float t = INFINITY;
  MaterialType mat{MaterialType::NONE};
  const Hittable *hit_object{nullptr};
  vec3 normal{};
  vec3 point{};
};

class Hittable {
public:
  Hittable(const MaterialType &m, const color &c, float fuzz = 0.0)
      : mat(m), albedo(c), fuzz(fuzz), pos({0, 0, 0}) {}
  virtual ~Hittable() = default;

  virtual bool hit(const ray &r, float ray_min, float ray_max,
                   HitRecord &rec) const = 0;

  void move(const point3 &new_pos) {
    pos = new_pos;
    update();
  }

  bool scatter(const ray &r_in, const HitRecord &rec, color &attenuation,
               ray &scattered) const {
    switch (mat) {
    case LAMBERTIAN:
      return scatter_lambertian(rec, attenuation, scattered);
    case METAL: {
      vec3 reflected = reflect(r_in.direction(), rec.normal);
      scattered = ray(rec.point, reflected);
      attenuation = albedo;

      return true;
    }
    case DIELECTRIC:
      return false; // TODO: Implement
    case EMMISSIVE:
      return false; // TODO: Implement
    case NONE:
      return false;
    };
  }

protected:
  point3 get_pos(void) const { return pos; }
  MaterialType get_material(void) const { return mat; };
  virtual void update(void) = 0;

  bool scatter_lambertian(const HitRecord &rec, color &attenuation,
                          ray &scattered) const {

    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.x <= math::K_EPSILON &&
        scatter_direction.x >= -math::K_EPSILON &&
        scatter_direction.y <= math::K_EPSILON &&
        scatter_direction.y >= -math::K_EPSILON &&
        scatter_direction.z <= math::K_EPSILON &&
        scatter_direction.z >= -math::K_EPSILON) {
      scatter_direction = rec.normal;
    }

    scattered = ray(rec.point, scatter_direction);
    attenuation = albedo;
    return true;
  }

private:
  MaterialType mat;
  color albedo;
  point3 pos;

  // how fuzzy a metal material is. 0 is a perfect mirror, and higher values are
  // more LAMBERTIAN
  float fuzz;
};
