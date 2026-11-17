#pragma once

#include "cube.h"
#include <cstddef>
#include <vector>

class Bvh {
public:
  void build(const std::vector<Cube> &cubes);
  bool requiresLinear(const ray &r) const;
  bool hit(const std::vector<Cube> &cubes, const ray &r, float ray_min,
           float ray_max, HitRecord &rec) const;

private:
  struct Bounds {
    vec3 min;
    vec3 max;
  };
  struct Node {
    Bounds bounds;
    size_t left = 0;
    size_t right = 0;
    size_t start = 0;
    size_t count = 0;
  };

  size_t buildNode(size_t start, size_t end);
  static bool intersect(const Bounds &bounds, const ray &r, float ray_min,
                        float ray_max, double padding, float &near_t);

  std::vector<Node> nodes;
  std::vector<size_t> indices;
  std::vector<Bounds> cube_bounds;
  std::vector<glm::mat3> inverse_directions;
  bool unsafe_transform = false;
  double max_condition = 1.0;
  double max_inverse_translation = 0.0;
};
