#include "camera.h"
#include "ray.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <random>

namespace {
glm::quat yawOrientation(float yaw) {
  return glm::angleAxis(-yaw, glm::vec3(0, 1, 0));
}

glm::quat cameraOrientation(float yaw, float pitch) {
  const glm::quat yawQuat = yawOrientation(yaw);
  const vec3 rightAxis = yawQuat * glm::vec3(1, 0, 0);
  const glm::quat pitchQuat = glm::angleAxis(-pitch, rightAxis);
  return pitchQuat * yawQuat;
}
} // namespace

Camera::Camera(uint32_t _width, uint32_t _height)
    : width(_width), height(_height), center(0, 0, 0), yaw(0.0f), pitch(0.0f) {
  std::random_device rd;
  rng = std::mt19937{rd()};
}

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
        const float x_offset =
            std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);
        const float y_offset =
            std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);

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
  if (depth <= 0)
    return color(0, 0, 0);

  HitRecord rec;

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, math::K_EPSILON, INFINITY, rec)) {
    vec3 unit_direction = glm::normalize(r.direction());
    float a = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }

  ray scattered;
  color attenuation;
  color color_from_emission;
  rec.hit_object->emitted(color_from_emission);

  if (!rec.hit_object->scatter(r, rec, attenuation, scattered))
    return color_from_emission;

  color color_from_scatter =
      attenuation * ray_color(scattered, world, depth - 1);

  return color_from_emission + color_from_scatter;
}
