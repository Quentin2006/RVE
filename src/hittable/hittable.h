#pragma once

#include "../consts.h"
#include "../ray.h"

enum class Material {
  Diffuse,
  Metal,
  Transparent,
  Emissive,
};

struct HitRecord {
  float t = INFINITY;
  vec3 normal{};
  vec3 point{};
};

class Hittable {
public:
  Hittable(Material m) : mat(m), pos({0, 0, 0}) {}
  virtual ~Hittable() = default;

  virtual bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const = 0;

  void move(const point3 &new_pos) {
    pos = new_pos;
    update();
  }

protected:
  point3 get_pos(void) const { return pos; }
  Material get_material(void) const { return mat; };
  virtual void update(void) = 0;

private:
  Material mat;
  point3 pos;
};
