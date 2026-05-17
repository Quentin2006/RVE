#include "triangle.h"

class Face : public Hittable {
public:
  Face(point3 p, vec3 u, vec3 v);

  bool is_hit(const ray &r, vec3 &normal) const override;

private:
  void update() override;

  vec3 A, B, C, D;
  Triangle top, bottom;
};
