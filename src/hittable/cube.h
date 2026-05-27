#include "hittable.h"
#include <glm/mat4x4.hpp>

class Cube : public Hittable {
public:
  Cube(const point3 &point, Material mat = Material::Diffuse);

  void setRotation(const vec3 &rotationRadians);
  void setScale(const vec3 &scaleFactors);

  bool hit(const ray &r, vec3 &normal, float &t) const override;

private:
  void update(void) override;

  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix;
  glm::mat4 inverseModelMatrix;
};
