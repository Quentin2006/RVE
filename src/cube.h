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
       float fuzz = 0.0f, float refraction_index = 1.5f)
      : mat(m), albedo(c), fuzz(fuzz), refraction_index(refraction_index),
        pos(point) {
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
               ray &scattered) const;

  void setRefractionIndex(float newRefractionIndex) {
    refraction_index = newRefractionIndex;
  }

private:
  float sanitizeScaleComponent(float value) {
    const float magnitude = std::max(std::abs(value), math::K_EPSILON);
    return std::copysign(magnitude, value == 0.0f ? 1.0f : value);
  }

  bool scatter_lambertian(const HitRecord &rec, color &attenuation,
                          ray &scattered) const;

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
  float brightness;

  vec3 pos;
  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix{1.0f};
  glm::mat4 inverseModelMatrix{1.0f};
  glm::mat3 normalMatrix{1.0f};
  std::array<vec3, 6> faceNormals;
};
