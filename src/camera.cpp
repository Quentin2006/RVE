#include "camera.h"
#include "ray.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>

namespace {
glm::quat yawOrientation(float yaw) {
  return glm::angleAxis(-yaw, glm::vec3(0, 1, 0));
}

glm::quat cameraOrientation(float yaw, float pitch) {
  // Yaw around world up (Y). Pitch must rotate around the camera's local
  // right axis AFTER applying yaw, not the fixed world X axis. Compute the
  // yaw-only quaternion, rotate the world right vector by it to get the
  // camera right axis, then build the pitch quaternion around that axis.
  const glm::quat yawQuat = yawOrientation(yaw);
  const glm::vec3 rightAxis = yawQuat * glm::vec3(1, 0, 0);
  const glm::quat pitchQuat = glm::angleAxis(-pitch, rightAxis);
  return pitchQuat * yawQuat;
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
  const glm::quat yawQuat = yawOrientation(yaw);
  const vec3 forward = yawQuat * vec3(0, 0, -1);
  center += forward * distance;
}

void Camera::moveRight(float distance) {
  const glm::quat yawQuat = yawOrientation(yaw);
  const vec3 right = yawQuat * vec3(1, 0, 0);
  center += right * distance;
}

void Camera::moveUp(float distance) { center += vec3(0, 1, 0) * distance; }

void Camera::render(const World &world,
                    std::array<uint32_t, window::SIZE> &pixels) const {

  const glm::quat orientation = cameraOrientation(yaw, pitch);
  const auto forward = orientation * vec3(0, 0, -1);
  const auto right = orientation * vec3(1, 0, 0);
  const auto up = orientation * vec3(0, 1, 0);
  const auto viewport_u = right * camera::VIEWPORT_WIDTH;
  const auto viewport_v = -up * camera::VIEWPORT_HEIGHT;
  const auto pixel_delta_u = viewport_u / static_cast<float>(width);
  const auto pixel_delta_v = viewport_v / static_cast<float>(height);
  const auto viewport_upper_left = center + forward * camera::FOCAL_LENGTH -
                                   viewport_u / 2.f - viewport_v / 2.f;
  const auto pixel00_loc =
      viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

  for (uint32_t j = 0; j < height; j++) {
    const auto row_start =
        pixel00_loc + (static_cast<float>(j) * pixel_delta_v);
    auto *row = pixels.data() + (j * width);
    auto pixel_center = row_start;
    for (uint32_t i = 0; i < width; i++) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < camera::SAMPLES_PER_PIXEL; s++) {
        const float x_offset = (static_cast<float>(rand()) / RAND_MAX - 0.5f);
        const float y_offset = (static_cast<float>(rand()) / RAND_MAX - 0.5f);

        auto sample_point =
            pixel_center + x_offset * pixel_delta_u + y_offset * pixel_delta_v;

        ray r(center, sample_point - center);

        pixel_color += ray_color(r, world, camera::MAX_DEPTH);
      }
      pixel_color /= static_cast<float>(camera::SAMPLES_PER_PIXEL);
      *row++ = convert(pixel_color);

      pixel_center += pixel_delta_u;
    }
  }
}

color Camera::ray_color(const ray &r, const World &world, int depth) const {
  HitRecord rec;
  if (world.hit(r, 0.f, INFINITY, rec)) {
    return 0.5f * (rec.normal + color(1.0f, 1.0f, 1.0f));
  }

  vec3 unit_direction = glm::normalize(r.direction());
  float a = 0.5f * (unit_direction.y + 1.0f);
  return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}
