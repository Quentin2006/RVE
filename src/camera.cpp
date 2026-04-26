#include "camera.h"
#include "consts.h"

Camera::Camera(uint32_t _width, uint32_t _height)
    : width(_width), height(_height), center(0, 0, 0) {
}

void Camera::moveForward(float distance) { center.z -= distance; }

void Camera::moveRight(float distance) { center.x += distance; }

void Camera::moveUp(float distance) { center.y += distance; }

void Camera::render(const World &world,
                    std::array<uint32_t, window::SIZE> &pixels) const {

  const auto viewport_u = glm::vec3(camera::VIEWPORT_WIDTH, 0, 0);
  const auto viewport_v = glm::vec3(0, -camera::VIEWPORT_HEIGHT, 0);
  const auto pixel_delta_u = viewport_u / static_cast<float>(width);
  const auto pixel_delta_v = viewport_v / static_cast<float>(height);
  const auto viewport_upper_left = center - vec3(0, 0, camera::FOCAL_LENGTH) -
                                   viewport_u / 2.f - viewport_v / 2.f;
  const auto pixel00_loc =
      viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

  for (uint32_t j = 0; j < window::HEIGHT; j++) {
    for (uint32_t i = 0; i < window::WIDTH; i++) {
      auto pixel_center =
          pixel00_loc + ((float)i * pixel_delta_u) + ((float)j * pixel_delta_v);
      auto ray_direction = pixel_center - center;
      ray r(center, ray_direction);

      color pixel_color = ray_color(r, world);

      pixels[j * window::WIDTH + i] = convert(pixel_color);
    }
  }
}

color Camera::ray_color(const ray &r, const World &world) const {

  for (const auto &cube : world) {
    vec3 normal = cube->is_hit(r);

    if (normal != vec3(0, 0, 0)) {
      return color(normal.x, normal.y, normal.z);
    }
  }

  vec3 unit_direction = glm::normalize(r.direction());
  float a = 0.5f * (unit_direction.y + 1.0f);
  return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}
