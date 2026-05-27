#include "../ray.h"
#include "hittable.h"

class Triangle : public Hittable {
public:
  Triangle(Material material);
  Triangle(Material material, point3 A, point3 B, point3 C);

  bool hit(const ray &r, vec3 &normal, float &t) const override;

private:
  bool is_hit(const ray &r, vec3 &normal, float &t) const;

  void update(void) override;

  point3 A, B, C;
  vec3 AB, BC, CA;
  vec3 N;
};
