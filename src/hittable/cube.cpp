#include "cube.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

namespace {

float sanitizeScaleComponent(float value) {
  const float magnitude = std::max(std::abs(value), math::K_EPSILON);
  return std::copysign(magnitude, value == 0.0f ? 1.0f : value);
}

ray transformRay(const ray &r, const glm::mat4 &m) {
  const glm::vec4 origin = m * glm::vec4(r.origin(), 1.0f);
  const glm::vec4 direction = m * glm::vec4(r.direction(), 0.0f);
  return ray(point3(origin), vec3(direction));
}

} // namespace

void Cube::setRotation(const vec3 &newRotationRadians) {
  rotationRadians = newRotationRadians;
  update();
}

void Cube::setScale(const vec3 &newScaleFactors) {
  scaleFactors = vec3(sanitizeScaleComponent(newScaleFactors.x),
                      sanitizeScaleComponent(newScaleFactors.y),
                      sanitizeScaleComponent(newScaleFactors.z));
  update();
}

bool Cube::hit(const ray &r, float ray_min, float ray_max,
               HitRecord &rec) const {
  const ray localRay = transformRay(r, inverseModelMatrix);
  const point3 minCorner(0.0f, 0.0f, 0.0f);
  const point3 maxCorner(1.0f, 1.0f, 1.0f);
  const auto &origin = localRay.origin();
  const auto &direction = localRay.direction();
  const bool originInside = origin.x > minCorner.x && origin.x < maxCorner.x &&
                            origin.y > minCorner.y && origin.y < maxCorner.y &&
                            origin.z > minCorner.z && origin.z < maxCorner.z;

  float tMin = -std::numeric_limits<float>::infinity();
  float tMax = std::numeric_limits<float>::infinity();
  int enterFace = 0;
  int exitFace = 0;

  for (int axis = 0; axis < 3; ++axis) {
    const float axisOrigin = origin[axis];
    const float axisDirection = direction[axis];
    const float slabMin = minCorner[axis];
    const float slabMax = maxCorner[axis];
    int nearFace = axis * 2;
    int farFace = axis * 2 + 1;

    if (std::abs(axisDirection) < math::K_EPSILON) {
      if (axisOrigin < slabMin || axisOrigin > slabMax) {
        return false;
      }
      continue;
    }

    const float invDirection = 1.0f / axisDirection;
    float t0 = (slabMin - axisOrigin) * invDirection;
    float t1 = (slabMax - axisOrigin) * invDirection;

    if (t0 > t1) {
      std::swap(t0, t1);
      std::swap(nearFace, farFace);
    }

    if (t0 > tMin) {
      tMin = t0;
      enterFace = nearFace;
    }
    if (t1 < tMax) {
      tMax = t1;
      exitFace = farFace;
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
  rec.normal = faceNormals[tMin >= ray_min ? enterFace : exitFace];
  rec.point = r.origin() + hitT * r.direction();
  rec.mat = get_material();
  rec.hit_object = this;

  return true;
}

void Cube::update(void) {
  modelMatrix = glm::translate(glm::mat4(1.0f), get_pos() + vec3(0.5f));
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
