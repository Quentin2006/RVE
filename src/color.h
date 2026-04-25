#include <cstdint>
#include <glm/glm.hpp>

using color = glm::vec3;

uint32_t convert(color &c) {
  return (static_cast<uint32_t>(c.r * 255.0f) << 16) |
         (static_cast<uint32_t>(c.g * 255.0f) << 8) |
         static_cast<uint32_t>(c.b * 255.0f);
}
