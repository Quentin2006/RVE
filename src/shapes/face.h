#include "triangle.h"

class Face : public Hittable {
public:
  Face(point3 p, vec3 u, vec3 v) : local_p(p), u(u), v(v) { move(p); }

  bool is_hit(const ray &r, vec3 &normal) const override;

private:
  void update() override {}

  point3 local_p;
  vec3 u, v;
};
