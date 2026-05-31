#include "../ray.h"
#include "hittable.h"

class Triangle final : public Hittable {
public:
  Triangle(MaterialType material, color albedo)
      : Hittable(material, albedo), A({0, 0, 0}), B({0, 0, 0}), C({0, 0, 0}) {
    move({0, 0, 0});
  }
  Triangle(MaterialType material, color albedo, point3 A, point3 B, point3 C)
      : Hittable(material, albedo), A(A), B(B), C(C) {}

  bool hit(const ray &r, float ray_min, float ray_max,
           HitRecord &rec) const override;

private:
  void update(void) override;

  point3 A, B, C;
  vec3 AB, AC;
  vec3 triangleNormal;
  bool valid{false};
};
