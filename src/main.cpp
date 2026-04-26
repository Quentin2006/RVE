#include "color.h"
#include "consts.h"
#include "ray.h"
#include "window.h"

#include <algorithm>
#include <array>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <iostream>

bool hit_cube(const point3 &center, const ray &r) {
  // 1. Compute the two t values where the ray crosses the two planes:
  //     t0 = (boxMin.axis - rayOrigin.axis) / rayDir.axis;
  //     t1 = (boxMax.axis - rayOrigin.axis) / rayDir.axis;
  //
  // 2. If needed, swap them so t0 is the entry and t1 is the exit:
  //     if (t0 > t1) swap(t0, t1);
  //
  // 3. Keep the overlap of all three axes:
  //     tmin = max(tmin, t0);
  //     tmax = min(tmax, t1);
  //
  // 4. If at any point tmax < tmin, the ray misses the cube.

  // calc the cubes x, y and z points, the size will always be 1x1
  // https://www.math.brown.edu/tbanchof/Beyond3d/chapter8/section01.html
  point3 boxMin(-0.5, -0.5, -0.5);
  point3 boxMax(0.5, 0.5, 0.5);
  boxMin -= center;
  boxMax -= center;

  float tmin = -std::numeric_limits<float>::infinity();
  float tmax = std::numeric_limits<float>::infinity();

  for (int i = 0; i < 3; ++i) {
    float dir = r.direction()[i];
    float orig = r.origin()[i];

    float t0 = (boxMin[i] - orig) / dir;
    float t1 = (boxMax[i] - orig) / dir;

    if (t0 > t1)
      std::swap(t0, t1);

    tmin = std::max(tmin, t0);
    tmax = std::min(tmax, t1);

    if (tmax < tmin) {
      return false;
    }
  }

  return true;
}

color ray_color(const ray &r) {

  if (hit_cube(point3(1, 1, -5), r))
    return color(1, 0, 0);

  vec3 unit_direction = glm::normalize(r.direction());
  float a = 0.5 * (unit_direction.y + 1.0);
  return (1.0f - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {

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
