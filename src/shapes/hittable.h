#pragma once

#include "../ray.h"
class Hittable {
public:
  Hittable() : pos({0, 0, 0}) {}

  virtual bool is_hit(const ray &r, vec3 &normal) const = 0;

  // NOTE: final means that this method cannot be overridden
  virtual void move(const point3 &new_pos) final {
    pos = new_pos;
    update();
  }

protected:
  virtual void update(void) = 0;
  virtual point3 get_pos(void) const final { return pos; }

private:
  point3 pos;
};
