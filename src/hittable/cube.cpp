#include "cube.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
constexpr float kEpsilon = 1e-6f;

float sanitizeScaleComponent(float value) {
  const float magnitude = std::max(std::abs(value), kEpsilon);
  return std::copysign(magnitude, value == 0.0f ? 1.0f : value);
}

ray transformRay(const ray &r, const glm::mat4 &m) {
  const glm::vec4 origin = m * glm::vec4(r.origin(), 1.0f);
  const glm::vec4 direction = m * glm::vec4(r.direction(), 0.0f);
  return ray(point3(origin), vec3(direction));
}

vec3 transformNormal(const vec3 &n, const glm::mat4 &inverseModelMatrix) {
  const glm::mat3 normalMatrix = glm::transpose(glm::mat3(inverseModelMatrix));
  return glm::normalize(normalMatrix * n);
}
}

Cube::Cube(const point3 &p, Material mat)
    : Hittable(mat), modelMatrix(1.0f), inverseModelMatrix(1.0f) {
  move(p);
}

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

bool Cube::hit(const ray &r, vec3 &normal, float &t) const {
  const ray localRay = transformRay(r, inverseModelMatrix);
  const point3 minCorner(0.0f, 0.0f, 0.0f);
  const point3 maxCorner(1.0f, 1.0f, 1.0f);

  float tMin = -std::numeric_limits<float>::infinity();
  float tMax = std::numeric_limits<float>::infinity();
  vec3 enterNormal(0.0f);
  vec3 exitNormal(0.0f);

  for (int axis = 0; axis < 3; ++axis) {
    const float origin = localRay.origin()[axis];
    const float direction = localRay.direction()[axis];
    const float slabMin = minCorner[axis];
    const float slabMax = maxCorner[axis];

    if (std::abs(direction) < kEpsilon) {
      if (origin < slabMin || origin > slabMax) {
        return false;
      }
      continue;
    }

    const float invDirection = 1.0f / direction;
    float t0 = (slabMin - origin) * invDirection;
    float t1 = (slabMax - origin) * invDirection;

    vec3 n0(0.0f);
    vec3 n1(0.0f);
    n0[axis] = -1.0f;
    n1[axis] = 1.0f;

    if (t0 > t1) {
      std::swap(t0, t1);
      std::swap(n0, n1);
    }

    if (t0 > tMin) {
      tMin = t0;
      enterNormal = n0;
    }
    if (t1 < tMax) {
      tMax = t1;
      exitNormal = n1;
    }

    if (tMin > tMax) {
      return false;
    }
  }

  if (tMax <= kEpsilon) {
    return false;
  }

  if (tMin > kEpsilon) {
    t = tMin;
    normal = enterNormal;
  } else {
    t = tMax;
    normal = exitNormal;
  }

  normal = transformNormal(normal, inverseModelMatrix);

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
}
