#include "hittable.h"
#include <array>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

class Cube final : public Hittable {
public:
  Cube(const point3 &point, MaterialType mat, color albedo, float fuzz = 0.0f)
      : Hittable(mat, albedo, fuzz), modelMatrix(1.0f),
        inverseModelMatrix(1.0f) {
    move(point);
  }

  void setRotation(const vec3 &rotationRadians);
  void setScale(const vec3 &scaleFactors);

  bool hit(const ray &r, float ray_min, float ray_max,
           HitRecord &rec) const override;

private:
  void update(void) override;

  vec3 rotationRadians{0.0f};
  vec3 scaleFactors{1.0f};
  glm::mat4 modelMatrix;
  glm::mat4 inverseModelMatrix;
  glm::mat3 normalMatrix;
  std::array<vec3, 6> faceNormals;
};
