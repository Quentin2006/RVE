#include "cube.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>
#include <random>

namespace {

ray transformRay(const ray &r, const glm::mat4 &m) {
  const glm::vec4 origin = m * glm::vec4(r.origin(), 1.0f);
  const glm::vec4 direction = m * glm::vec4(r.direction(), 0.0f);
  return ray(vec3(origin), vec3(direction));
}

inline vec3 random_unit_vector() {
  glm::vec3 random_direction = glm::sphericalRand(1.0f);
  return glm::normalize(random_direction);
}
inline float random_float() {
  static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
  static std::mt19937 generator;
  return distribution(generator);
}

inline bool close_to_zero(glm::vec3 v) {
  return (std::abs(v.x) < math::K_EPSILON) &&
         (std::abs(v.y) < math::K_EPSILON) && (std::abs(v.z) < math::K_EPSILON);
}
} // namespace

bool Cube::hit(const ray &r, float ray_min, float ray_max,
               HitRecord &rec) const {
  const ray localRay = transformRay(r, inverseModelMatrix);
  const vec3 minCorner(0.0f, 0.0f, 0.0f);
  const vec3 maxCorner(1.0f, 1.0f, 1.0f);
  const auto &origin = localRay.origin();
  const auto &direction = localRay.direction();
  const bool originInside = origin.x > minCorner.x && origin.x < maxCorner.x &&
                            origin.y > minCorner.y && origin.y < maxCorner.y &&
                            origin.z > minCorner.z && origin.z < maxCorner.z;

  float tMin = -std::numeric_limits<float>::infinity();
  float tMax = std::numeric_limits<float>::infinity();
  uint enterFace = 0;
  uint exitFace = 0;

  for (int axis = 0; axis < 3; ++axis) {
    const float axisOrigin = origin[axis];
    const float axisDirection = direction[axis];
    const float slabMin = minCorner[axis];
    const float slabMax = maxCorner[axis];
    const int nearFace = axis * 2;
    const int farFace = axis * 2 + 1;

    if (std::abs(axisDirection) < math::K_EPSILON) {
      if (axisOrigin < slabMin || axisOrigin > slabMax) {
        return false;
      }
      continue;
    }

    const float invDirection = 1.0f / axisDirection;
    float t0 = (slabMin - axisOrigin) * invDirection;
    float t1 = (slabMax - axisOrigin) * invDirection;

    const int useNearFace = t0 > t1 ? farFace : nearFace;
    const int useFarFace = t0 > t1 ? nearFace : farFace;
    if (t0 > t1) {
      std::swap(t0, t1);
    }

    if (t0 > tMin) {
      tMin = t0;
      enterFace = static_cast<uint>(useNearFace);
    }
    if (t1 < tMax) {
      tMax = t1;
      exitFace = static_cast<uint>(useFarFace);
    }

    if (tMin > tMax || tMin > ray_max) {
      return false;
    }
  }

  if (tMax <= ray_min || tMin > ray_max) {
    return false;
  }

  if (tMin < ray_min && !originInside) {
    return false;
  }

  float hitT = tMin >= ray_min ? tMin : tMax;
  rec.t = hitT;
  rec.point = r.origin() + hitT * r.direction();
  rec.mat = data.mat;
  rec.hit_object = this;
  rec.normal = faceNormals[originInside ? exitFace : enterFace];
  rec.front_face = dot(r.direction(), rec.normal) < 0;

  return true;
}

void Cube::update() {
  modelMatrix = glm::translate(glm::mat4(1.0f), data.pos + vec3(0.5f));
  modelMatrix = glm::rotate(modelMatrix, rotationRadians.x, vec3(1, 0, 0));
  modelMatrix = glm::rotate(modelMatrix, rotationRadians.y, vec3(0, 1, 0));
  modelMatrix = glm::rotate(modelMatrix, rotationRadians.z, vec3(0, 0, 1));
  modelMatrix = glm::scale(modelMatrix, scaleFactors);
  modelMatrix = glm::translate(modelMatrix, vec3(-0.5f, -0.5f, -0.5f));
  inverseModelMatrix = glm::inverse(modelMatrix);
  normalMatrix = glm::transpose(glm::mat3(inverseModelMatrix));
  faceNormals[0] = glm::normalize(normalMatrix * vec3(-1.0f, 0.0f, 0.0f));
  faceNormals[1] = glm::normalize(normalMatrix * vec3(1.0f, 0.0f, 0.0f));
  faceNormals[2] = glm::normalize(normalMatrix * vec3(0.0f, -1.0f, 0.0f));
  faceNormals[3] = glm::normalize(normalMatrix * vec3(0.0f, 1.0f, 0.0f));
  faceNormals[4] = glm::normalize(normalMatrix * vec3(0.0f, 0.0f, -1.0f));
  faceNormals[5] = glm::normalize(normalMatrix * vec3(0.0f, 0.0f, 1.0f));
}

bool Cube::scatter(const ray &r_in, const HitRecord &rec, color &attenuation,
                   ray &scattered) const {
  switch (data.mat) {
  case LAMBERTIAN:
    return scatter_lambertian(rec, attenuation, scattered);
  case METAL: {
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = glm::normalize(reflected) + (data.fuzz * random_unit_vector());
    scattered = ray(rec.point, reflected);
    attenuation = data.albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }
  case DIELECTRIC: {
    attenuation = data.albedo;
    float ri =
        rec.front_face ? (1.0f / data.refraction_index) : data.refraction_index;

    vec3 unit_direction = glm::normalize(r_in.direction());
    float cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0f);
    float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_float())
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, ri);

    scattered = ray(rec.point, direction);

    return true;
  }
  default:
    return false;
  };
}

bool Cube::scatter_lambertian(const HitRecord &rec, color &attenuation,
                              ray &scattered) const {
  auto scatter_direction = rec.normal + random_unit_vector();

  if (close_to_zero(scatter_direction)) {
    scatter_direction = rec.normal;
  }

  scattered = ray(rec.point, scatter_direction);
  attenuation = data.albedo;
  return true;
}
