#include "camera.h"
#include "consts.h"
#include "ray.h"
#include <glm/gtc/quaternion.hpp>

namespace {
glm::quat cameraOrientation(float yaw, float pitch) {
  // Yaw around world up (Y). Pitch must rotate around the camera's local
  // right axis AFTER applying yaw, not the fixed world X axis. Compute the
  // yaw-only quaternion, rotate the world right vector by it to get the
  // camera right axis, then build the pitch quaternion around that axis.
  const glm::quat yawQuat = glm::angleAxis(-yaw, glm::vec3(0, 1, 0));
  const glm::vec3 rightAxis = yawQuat * glm::vec3(1, 0, 0);
  const glm::quat pitchQuat = glm::angleAxis(-pitch, rightAxis);
  return pitchQuat * yawQuat;
}

vec3 cameraForward(float yaw, float pitch) {
  return cameraOrientation(yaw, pitch) * vec3(0, 0, -1);
}

vec3 cameraRight(float yaw, float pitch) {
  return cameraOrientation(yaw, pitch) * vec3(1, 0, 0);
}

vec3 cameraUp(float yaw, float pitch) {
  return cameraOrientation(yaw, pitch) * vec3(0, 1, 0);
}
} // namespace

Camera::Camera(uint32_t _width, uint32_t _height)
    : width(_width), height(_height), center(0, 0, 0), yaw(0.0f), pitch(0.0f) {}

void Camera::rotateYaw(float delta) { yaw += delta; }

void Camera::rotatePitch(float delta) {
  pitch += delta;
  const float maxPitch = glm::radians(89.0f);
  const float minPitch = -maxPitch;
  if (pitch > maxPitch)
    pitch = maxPitch;
  if (pitch < minPitch)
    pitch = minPitch;
}

void Camera::moveForward(float distance) {
  const glm::quat yawQuat = glm::angleAxis(-yaw, glm::vec3(0, 1, 0));
  const vec3 forward = yawQuat * vec3(0, 0, -1);
  center += forward * distance;
}

void Camera::moveRight(float distance) {
  const glm::quat yawQuat = glm::angleAxis(-yaw, glm::vec3(0, 1, 0));
  const vec3 right = yawQuat * vec3(1, 0, 0);
  center += right * distance;
}

void Camera::moveUp(float distance) {
  center += vec3(0, 1, 0) * distance;
}

void Camera::render(const World &world,
                    std::array<uint32_t, window::SIZE> &pixels) const {

  const auto forward = cameraForward(yaw, pitch);
  const auto right = cameraRight(yaw, pitch);
  const auto up = cameraUp(yaw, pitch);
  const auto viewport_u = right * camera::VIEWPORT_WIDTH;
  const auto viewport_v = -up * camera::VIEWPORT_HEIGHT;
  const auto pixel_delta_u = viewport_u / static_cast<float>(width);
  const auto pixel_delta_v = viewport_v / static_cast<float>(height);
  const auto viewport_upper_left = center + forward * camera::FOCAL_LENGTH -
                                   viewport_u / 2.f - viewport_v / 2.f;
  const auto pixel00_loc =
      viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

  for (uint32_t j = 0; j < height; j++) {
    for (uint32_t i = 0; i < width; i++) {
      auto pixel_center =
          pixel00_loc + ((float)i * pixel_delta_u) + ((float)j * pixel_delta_v);
      auto ray_direction = pixel_center - center;
      ray r(center, ray_direction);

      color pixel_color = ray_color(r, world);

      pixels[j * width + i] = convert(pixel_color);
    }
  }
}

color Camera::ray_color(const ray &r, const World &world) const {

  for (const auto &cube : world) {
    vec3 normal;
    float t;
    if (cube->hit(r, normal, t)) {
      return 0.5f * (normal + color(1.0f, 1.0f, 1.0f));
    }
  }

  vec3 unit_direction = glm::normalize(r.direction());
  float a = 0.5f * (unit_direction.y + 1.0f);
  return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}
