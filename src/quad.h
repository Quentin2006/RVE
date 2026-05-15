#include "ray.h"

class Quad {
public:
  Quad(vec3 point, vec3 normal);

  bool is_hit(const ray &r) const;
  // void move(vec3 delta) { center += delta; }

private:
  vec3 normal;
  point3 point;
  vec3 u;
  vec3 v;
};
