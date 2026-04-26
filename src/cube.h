#include "ray.h"
#include <glm/glm.hpp>

class Cube {
public:
  Cube(float x, float y, float z);

  /**
   * @brief will check if ray hits cube
   *
   * @param r ray being casted
   * @return (0,0,0) if not hit, or normal vec if is hit
   */
  vec3 is_hit(const ray &r) const;

private:
  point3 center;
};
