#include "cube.h"

Cube::Cube(float x, float y, float z) : center(x, y, z) {}

vec3 Cube::is_hit(const ray &r) const {
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
  const float eps = 0.0001f;
  point3 boxMin(-0.5, -0.5, -0.5);
  point3 boxMax(0.5, 0.5, 0.5);
  boxMin -= center;
  boxMax -= center;

  float tmin = -std::numeric_limits<float>::infinity();
  float tmax = std::numeric_limits<float>::infinity();

  vec3 normal(0, 0, 0);
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
      return vec3(0, 0, 0);
    }

    float tHit;
    if (tmin > 0) {
      tHit = tmin;
    } else {
      tHit = tmax;
    }
    point3 p = r.at(tHit);
    if (std::abs(p.x - boxMin.x) < eps)
      normal = vec3(-1, 0, 0);
    else if (std::abs(p.x - boxMax.x) < eps)
      normal = vec3(1, 0, 0);
    else if (std::abs(p.y - boxMin.y) < eps)
      normal = vec3(0, -1, 0);
    else if (std::abs(p.y - boxMax.y) < eps)
      normal = vec3(0, 1, 0);
    else if (std::abs(p.z - boxMin.z) < eps)
      normal = vec3(0, 0, -1);
    else if (std::abs(p.z - boxMax.z) < eps)
      normal = vec3(0, 0, 1);
  }

  return normal;
}
