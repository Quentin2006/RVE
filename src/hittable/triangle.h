#include "../ray.h"
#include "hittable.h"

class Triangle final : public Hittable {
public:
  Triangle(Material material);
  Triangle(Material material, point3 A, point3 B, point3 C);

  bool hit(const ray &r, vec3 &normal, float &t, float t_max) const override;

private:
  void update(void) override;

  point3 A, B, C;
  vec3 AB, AC;
  vec3 triangleNormal;
  bool valid{false};
};
