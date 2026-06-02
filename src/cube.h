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

enum MaterialType { LAMBERTIAN, METAL, DIELECTRIC, EMISSIVE, NONE };

struct HitRecord {
  float t = INFINITY;
  MaterialType mat{MaterialType::NONE};
  const class Cube *hit_object{nullptr};
  vec3 normal{};
  vec3 point{};
  bool front_face;
};

struct VoxelData {
  MaterialType mat;
  color albedo;

  float fuzz{0.f};
  float refraction_index{1.5f};
  float brightness{1.f};

  vec3 pos;
};

class Cube {
public:
  Cube(const VoxelData &voxel) : data(voxel) { update(); }

  bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const;

  void move(const vec3 &new_pos) {
    data.pos = new_pos;
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

  bool emitted(color &c) const {
    if (data.mat == MaterialType::EMISSIVE) {
      c = data.albedo * data.brightness;
      return true;
    }
    c = vec3{0, 0, 0};
    return false;
  }

  void setRefractionIndex(float newRefractionIndex) {
    data.refraction_index = newRefractionIndex;
  }

  const VoxelData &getVoxelData() const { return data; }
  VoxelData &getVoxelData() { return data; }

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

  VoxelData data;

  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix{1.0f};
  glm::mat4 inverseModelMatrix{1.0f};
  glm::mat3 normalMatrix{1.0f};
  std::array<vec3, 6> faceNormals;
};
