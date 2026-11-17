#include "bvh.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>

void Bvh::build(const std::vector<Cube> &cubes) {
  nodes.clear();
  indices.resize(cubes.size());
  cube_bounds.clear();
  inverse_directions.clear();
  unsafe_transform = false;
  max_condition = 1.0;
  max_inverse_translation = 0.0;
  cube_bounds.reserve(cubes.size());
  std::iota(indices.begin(), indices.end(), size_t{0});

  const float infinity = std::numeric_limits<float>::infinity();
  for (const Cube &cube : cubes) {
    const glm::mat4 inverse = glm::inverse(cube.modelMatrixRef());
    const glm::mat3 inverse_direction(inverse);
    float max_model = 0.0f;
    float max_inverse = 0.0f;
    for (int column = 0; column < 4; ++column) {
      for (int row = 0; row < 4; ++row) {
        if (!std::isfinite(cube.modelMatrixRef()[column][row]) ||
            !std::isfinite(inverse[column][row])) {
          unsafe_transform = true;
        }
        if (column < 3 && row < 3) {
          max_model = std::max(max_model,
                               std::abs(cube.modelMatrixRef()[column][row]));
          max_inverse = std::max(max_inverse, std::abs(inverse[column][row]));
        }
        if (column == 3 && row < 3) {
          max_inverse_translation = std::max(
              max_inverse_translation,
              std::abs(static_cast<double>(inverse[column][row])));
        }
      }
    }
    // Avoid relying on forward bounds for poorly conditioned transforms.
    if (max_model * max_inverse > 1000.0f) unsafe_transform = true;
    max_condition = std::max(max_condition, static_cast<double>(max_model) *
                                            static_cast<double>(max_inverse));
    bool known = false;
    for (const glm::mat3 &existing : inverse_directions) {
      if (existing == inverse_direction) {
        known = true;
        break;
      }
    }
    if (!known) inverse_directions.push_back(inverse_direction);

    Bounds bounds{vec3(infinity), vec3(-infinity)};
    for (int x = 0; x != 2; ++x) {
      for (int y = 0; y != 2; ++y) {
        for (int z = 0; z != 2; ++z) {
          const vec3 corner(cube.modelMatrixRef() *
                            glm::vec4(static_cast<float>(x),
                                      static_cast<float>(y),
                                      static_cast<float>(z), 1.0f));
          for (int axis = 0; axis < 3; ++axis) {
            if (!std::isfinite(corner[axis])) unsafe_transform = true;
          }
          bounds.min = glm::min(bounds.min, corner);
          bounds.max = glm::max(bounds.max, corner);
        }
      }
    }
    // Account for rounding in the forward and inverse transforms.
    const vec3 padding = (glm::max(glm::abs(bounds.min), glm::abs(bounds.max)) +
                          vec3(1.0f)) *
                         (16.0f * std::numeric_limits<float>::epsilon());
    bounds.min -= padding;
    bounds.max += padding;
    for (int axis = 0; axis < 3; ++axis) {
      if (!std::isfinite(bounds.min[axis]) || !std::isfinite(bounds.max[axis]))
        unsafe_transform = true;
    }
    cube_bounds.push_back(bounds);
  }
  if (!indices.empty() && !unsafe_transform) {
    buildNode(0, indices.size());
  }
}

bool Bvh::requiresLinear(const ray &r) const {
  if (unsafe_transform) return true;
  for (int axis = 0; axis < 3; ++axis) {
    if (!std::isfinite(r.origin()[axis]) || !std::isfinite(r.direction()[axis]))
      return true;
  }
  // Cube::hit ignores any local direction component smaller than K_EPSILON.
  // Such a slab is effectively unbounded, so a finite world AABB cannot safely
  // reject it. Check each distinct inverse linear transform, with an error
  // margin for the floating-point matrix multiplication used by Cube::hit.
  for (const glm::mat3 &inverse : inverse_directions) {
    for (int row = 0; row < 3; ++row) {
      double component = 0.0;
      double magnitude = 0.0;
      double origin_magnitude = max_inverse_translation;
      for (int column = 0; column < 3; ++column) {
        const double product = static_cast<double>(inverse[column][row]) *
                               static_cast<double>(r.direction()[column]);
        component += product;
        magnitude += std::abs(product);
        origin_magnitude += std::abs(static_cast<double>(inverse[column][row]) *
                                     static_cast<double>(r.origin()[column]));
      }
      const double margin =
          8.0 * std::numeric_limits<float>::epsilon() * magnitude;
      if (std::max(magnitude, origin_magnitude) >
          static_cast<double>(std::numeric_limits<float>::max()) / 8.0)
        return true;
      if (std::abs(component) <= static_cast<double>(math::K_EPSILON) + margin)
        return true;
    }
  }
  return false;
}

size_t Bvh::buildNode(size_t start, size_t end) {
  const float infinity = std::numeric_limits<float>::infinity();
  Bounds bounds{vec3(infinity), vec3(-infinity)};
  vec3 centroid_min(infinity);
  vec3 centroid_max(-infinity);
  for (size_t i = start; i < end; ++i) {
    const Bounds &cube = cube_bounds[indices[i]];
    bounds.min = glm::min(bounds.min, cube.min);
    bounds.max = glm::max(bounds.max, cube.max);
    const vec3 centroid = (cube.min + cube.max) * 0.5f;
    centroid_min = glm::min(centroid_min, centroid);
    centroid_max = glm::max(centroid_max, centroid);
  }
  const size_t node_index = nodes.size();
  nodes.push_back(Node{bounds});
  const size_t count = end - start;
  if (count <= 4) {
    nodes[node_index].start = start;
    nodes[node_index].count = count;
    return node_index;
  }

  const vec3 extent = centroid_max - centroid_min;
  int axis = 0;
  if (extent.y > extent.x) axis = 1;
  if (extent.z > extent[axis]) axis = 2;
  const size_t middle = start + count / 2;
  std::nth_element(indices.begin() + static_cast<std::ptrdiff_t>(start),
                   indices.begin() + static_cast<std::ptrdiff_t>(middle),
                   indices.begin() + static_cast<std::ptrdiff_t>(end),
                   [&](size_t a, size_t b) {
                     const float ca = cube_bounds[a].min[axis] +
                                      cube_bounds[a].max[axis];
                     const float cb = cube_bounds[b].min[axis] +
                                      cube_bounds[b].max[axis];
                     return ca == cb ? a < b : ca < cb;
                   });
  const size_t left = buildNode(start, middle);
  const size_t right = buildNode(middle, end);
  nodes[node_index].left = left;
  nodes[node_index].right = right;
  return node_index;
}

bool Bvh::intersect(const Bounds &bounds, const ray &r, float ray_min,
                    float ray_max, double padding, float &near_t) {
  double near = ray_min;
  double far = ray_max;
  for (int axis = 0; axis < 3; ++axis) {
    const double origin = r.origin()[axis];
    const double direction = r.direction()[axis];
    const double slab_min = static_cast<double>(bounds.min[axis]) - padding;
    const double slab_max = static_cast<double>(bounds.max[axis]) + padding;
    if (direction == 0.0f) {
      if (origin < slab_min || origin > slab_max) return false;
      continue;
    }
    double a = (slab_min - origin) / direction;
    double b = (slab_max - origin) / direction;
    if (a > b) std::swap(a, b);
    near = std::max(near, a);
    far = std::min(far, b);
    if (near > far) return false;
  }
  near_t = std::nextafter(static_cast<float>(near),
                          -std::numeric_limits<float>::infinity());
  return true;
}

bool Bvh::hit(const std::vector<Cube> &cubes, const ray &r, float ray_min,
              float ray_max, HitRecord &rec) const {
  if (nodes.empty()) return false;
  struct Entry { size_t node; float near; };
  // Median splitting bounds the stack depth by the number of bits in size_t.
  std::array<Entry, std::numeric_limits<size_t>::digits + 1> stack;
  size_t stack_size = 0;
  // The local-space ray transform rounds both origin and direction; at large
  // distances its error can exceed a fixed AABB padding. Scale the padding
  // with the coordinate magnitude and transform conditioning.
  double coordinate_scale = 1.0;
  for (int axis = 0; axis < 3; ++axis) {
    coordinate_scale = std::max(coordinate_scale,
                                std::abs(static_cast<double>(r.origin()[axis])));
    coordinate_scale = std::max(coordinate_scale,
                                std::abs(static_cast<double>(nodes[0].bounds.min[axis])));
    coordinate_scale = std::max(coordinate_scale,
                                std::abs(static_cast<double>(nodes[0].bounds.max[axis])));
  }
  const double padding = 128.0 * std::numeric_limits<float>::epsilon() *
                         (max_condition + 1.0) * (max_condition + 1.0) *
                         coordinate_scale;
  float root_near;
  if (!intersect(nodes[0].bounds, r, ray_min, ray_max, padding, root_near)) return false;
  stack[stack_size++] = {0, root_near};
  bool found = false;
  float closest = ray_max;
  size_t closest_index = cubes.size();
  while (stack_size != 0) {
    const Entry entry = stack[--stack_size];
    if (entry.near > closest) continue;
    const Node &node = nodes[entry.node];
    if (node.count != 0) {
      for (size_t i = node.start; i < node.start + node.count; ++i) {
        const size_t index = indices[i];
        HitRecord candidate;
        if (cubes[index].hit(r, ray_min, closest, candidate) &&
            (candidate.t < closest ||
             (found && candidate.t == closest && index < closest_index))) {
          found = true;
          closest = candidate.t;
          closest_index = index;
          rec = candidate;
        }
      }
      continue;
    }
    float left_near, right_near;
    const bool left_hit = intersect(nodes[node.left].bounds, r, ray_min,
                                    closest, padding, left_near);
    const bool right_hit = intersect(nodes[node.right].bounds, r, ray_min,
                                     closest, padding, right_near);
    if (left_hit && right_hit) {
      if (left_near <= right_near) {
        stack[stack_size++] = {node.right, right_near};
        stack[stack_size++] = {node.left, left_near};
      } else {
        stack[stack_size++] = {node.left, left_near};
        stack[stack_size++] = {node.right, right_near};
      }
    } else if (left_hit) {
      stack[stack_size++] = {node.left, left_near};
    } else if (right_hit) {
      stack[stack_size++] = {node.right, right_near};
    }
  }
  return found;
}
