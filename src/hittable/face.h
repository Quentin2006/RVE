#include "hittable.h"

class Face : public Hittable {
public:
  Face(Material mat, point3 p, vec3 u, vec3 v)
      : Hittable(mat), u(u), v(v) { move(p); }

  bool hit(const ray &r, vec3 &normal, float &t) const override;

private:
  void update() override {}

  vec3 u, v;
};
