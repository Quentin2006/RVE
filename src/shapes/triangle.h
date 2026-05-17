#include "../ray.h"
#include "hittable.h"

class Triangle : public Hittable {
public:
  Triangle();
  Triangle(point3 A, point3 B, point3 C);

  bool is_hit(const ray &r, vec3 &normal) const override;

private:
  void update(void) override;

  point3 A, B, C;
  vec3 AB, BC, CA;
  vec3 N;
};
