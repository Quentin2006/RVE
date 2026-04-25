#include <cstdint>
#include <glm/glm.hpp>

class color : public glm::vec3 {

public:
  // this will inherit vecs constructor
  using glm::vec3::vec;

  /**
   * @brief will convert color to a hex value
   *
   * @param color float [0-1]
   * @return hex value
   */
  uint32_t convert() const {
    return (static_cast<uint32_t>(r * 255.0f) << 16) |
           (static_cast<uint32_t>(g * 255.0f) << 8) |
           static_cast<uint32_t>(b * 255.0f);
  }
};
