#pragma once

#include "../ray.h"

enum class Material {
  Diffuse,
  Metal,
  Transparent,
  Emissive,
};

class Hittable {
public:
  Hittable(Material m) : mat(m), pos({0, 0, 0}) {}
  virtual ~Hittable() = default;

  virtual bool hit(const ray &r, vec3 &normal, float &t) const = 0;

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
