#pragma once

#include "color.h"
#include "consts.h"
#include "ray.h"

#include <array>
#include <glm/geometric.hpp>
#include <glm/gtc/random.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

inline vec3 random_unit_vector() {
  glm::vec3 random_direction = glm::sphericalRand(1.0f);
  return glm::normalize(random_direction);
}

enum MaterialType { LAMBERTIAN, METAL, DIELECTRIC, EMMISSIVE, NONE };

struct HitRecord {
  float t = INFINITY;
  MaterialType mat{MaterialType::NONE};
  const class Cube *hit_object{nullptr};
  vec3 normal{};
  vec3 point{};
};

class Cube {
public:
  Cube(const vec3 &point, const MaterialType &m, const color &c, float fuzz = 0.0)
      : mat(m), albedo(c), fuzz(fuzz), pos(point), modelMatrix(1.0f),
        inverseModelMatrix(1.0f) {
    update();
  }

  bool hit(const ray &r, float ray_min, float ray_max,
           HitRecord &rec) const;

  void move(const vec3 &new_pos) { pos = new_pos; update(); }

  void setRotation(const vec3 &newRotation) {
    rotationRadians = newRotation;
    update();
  }

  void setScale(const vec3 &newScale) {
    scaleFactors = vec3(sanitizeScaleComponent(newScale.x),
                        sanitizeScaleComponent(newScale.y),
                        sanitizeScaleComponent(newScale.z));
    update();
  }

  bool scatter(const ray &r_in, const HitRecord &rec, color &attenuation,
               ray &scattered) const {
    switch (mat) {
    case LAMBERTIAN:
      return scatter_lambertian(rec, attenuation, scattered);
    case METAL: {
      vec3 reflected = reflect(r_in.direction(), rec.normal);
      reflected = glm::normalize(reflected) + (fuzz * random_unit_vector());
      scattered = ray(rec.point, reflected);
      attenuation = albedo;
      return (dot(scattered.direction(), rec.normal) > 0);
    }
    case DIELECTRIC:
      return false; // TODO: Implement
    case EMMISSIVE:
      return false; // TODO: Implement
    case NONE:
      return false;
    default:
      return false;
    };
  }

private:
  float sanitizeScaleComponent(float value) {
    const float magnitude = std::max(std::abs(value), math::K_EPSILON);
    return std::copysign(magnitude, value == 0.0f ? 1.0f : value);
  }

  bool scatter_lambertian(const HitRecord &rec, color &attenuation,
                          ray &scattered) const {
    auto scatter_direction = rec.normal + random_unit_vector();

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

  void update();

  MaterialType mat;
  color albedo;
  float fuzz;
  vec3 pos;
  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix;
  glm::mat4 inverseModelMatrix;
  glm::mat3 normalMatrix;
  std::array<vec3, 6> faceNormals;
};
