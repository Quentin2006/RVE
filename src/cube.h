#pragma once

#include "color.h"
#include "consts.h"
#include "ray.h"

#include <array>
#include <cstdlib>
#include <glm/geometric.hpp>
#include <glm/gtc/random.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

inline vec3 random_unit_vector() {
  glm::vec3 random_direction = glm::sphericalRand(1.0f);
  return glm::normalize(random_direction);
}
inline float random_float() { return glm::linearRand(0.0f, 1.0f); }

enum MaterialType { LAMBERTIAN, METAL, DIELECTRIC, EMMISSIVE, NONE };

struct HitRecord {
  float t = INFINITY;
  MaterialType mat{MaterialType::NONE};
  const class Cube *hit_object{nullptr};
  vec3 normal{};
  vec3 point{};
  bool front_face;
};

class Cube {
public:
  Cube(const vec3 &point, const MaterialType &m, const color &c,
       float fuzz = 0.0, float refraction_index = 1.0f)
      : mat(m), albedo(c), fuzz(fuzz), refraction_index(refraction_index),
        pos(point), modelMatrix(1.0f), inverseModelMatrix(1.0f) {
    update();
  }

  bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const;

  void move(const vec3 &new_pos) {
    pos = new_pos;
    update();
  }

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
    case DIELECTRIC: {
      attenuation = albedo;
      float ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

      vec3 unit_direction = glm::normalize(r_in.direction());
      float cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
      float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

      bool cannot_refract = ri * sin_theta > 1.0;
      vec3 direction;

      if (cannot_refract || reflectance(cos_theta, ri) > random_float())
        direction = reflect(unit_direction, rec.normal);
      else
        direction = refract(unit_direction, rec.normal, ri);

      scattered = ray(rec.point, direction);

      return true;
    }
    case EMMISSIVE:
      return false; // TODO: Implement
    case NONE:
      return false;
    };

    return false;
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

  float reflectance(double cosine, double refraction_index) const {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
  void update();

  MaterialType mat;
  color albedo;
  float fuzz;
  float refraction_index;
  vec3 pos;
  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix;
  glm::mat4 inverseModelMatrix;
  glm::mat3 normalMatrix;
  std::array<vec3, 6> faceNormals;
};
