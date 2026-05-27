#pragma once

#include <algorithm>
#include <cstdint>
#include <glm/glm.hpp>

using color = glm::vec3;

inline uint32_t convert(const color &c) {
  const auto r = static_cast<uint32_t>(255.999f * std::clamp(c.r, 0.0f, 1.0f));
  const auto g = static_cast<uint32_t>(255.999f * std::clamp(c.g, 0.0f, 1.0f));
  const auto b = static_cast<uint32_t>(255.999f * std::clamp(c.b, 0.0f, 1.0f));
  return (0xFFu << 24) | (r << 16) | (g << 8) | b;
}
