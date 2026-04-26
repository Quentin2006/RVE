#include "color.h"
#include "consts.h"
#include "cube.h"
#include "ray.h"
#include "window.h"

#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

std::vector<Cube> scene;

color ray_color(const ray &r) {

  for (const auto &cube : scene) {
    vec3 normal = cube.is_hit(r);

    if (normal != vec3(0, 0, 0)) {
      return color(normal.x, normal.y, normal.z);
    }
  }

  vec3 unit_direction = glm::normalize(r.direction());
  float a = 0.5 * (unit_direction.y + 1.0);
  return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {

  scene.push_back(Cube(0, 0, -5));
  scene.push_back(Cube(0, 1, -5));
  scene.push_back(Cube(0, 2, -5));
  scene.push_back(Cube(1, 1, -5));

  Window window{window::WIDTH, window::HEIGHT};

  std::array<uint32_t, window::SIZE> pixels;

  point3 camera_center(0, 0, 0);

  // Calculate the vectors across the horizontal and down the vertical viewport
  // edges.
  auto viewport_u = glm::vec3(camera::VIEWPORT_WIDTH, 0, 0);
  auto viewport_v = glm::vec3(0, -camera::VIEWPORT_HEIGHT, 0);

  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  auto pixel_delta_u = viewport_u / static_cast<float>(window::WIDTH);
  auto pixel_delta_v = viewport_v / static_cast<float>(window::HEIGHT);

  // Calculate the location of the upper left pixel.
  auto viewport_upper_left = camera_center - vec3(0, 0, camera::FOCAL_LENGTH) -
                             viewport_u / 2.f - viewport_v / 2.f;
  auto pixel00_loc =
      viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

  while (!window.should_close()) {

    for (int j = 0; j < window::HEIGHT; j++) {
      std::clog << "\rScanlines remaining: " << (window::HEIGHT - j) << ' '
                << std::flush;
      for (int i = 0; i < window::WIDTH; i++) {
        auto pixel_center = pixel00_loc + ((float)i * pixel_delta_u) +
                            ((float)j * pixel_delta_v);
        auto ray_direction = pixel_center - camera_center;
        ray r(camera_center, ray_direction);

        color pixel_color = ray_color(r);

        pixels[j * window::WIDTH + i] = convert(pixel_color);
      }
    }

    window.present(pixels);
  }

  return EXIT_SUCCESS;
}
