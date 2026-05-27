#include "hittable.h"

class Face final : public Hittable {
public:
  Face(Material mat, point3 p, vec3 u, vec3 v)
      : Hittable(mat), u(u), v(v) { move(p); }

  bool hit(const ray &r, vec3 &normal, float &t, float t_max) const override;

private:
  void update() override;

  point3 origin;
  vec3 u, v;
  vec3 faceNormal;
  float uu{};
  float uv{};
  float vv{};
  float invDet{};
  bool valid{false};
};
