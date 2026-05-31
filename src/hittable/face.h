#include "hittable.h"

class Face final : public Hittable {
public:
  Face(MaterialType mat, color albedo, point3 p, vec3 u, vec3 v)
      : Hittable(mat, albedo), u(u), v(v) {
    move(p);
  }

  bool hit(const ray &r, float ray_min, float ray_max,
           HitRecord &rec) const override;

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
