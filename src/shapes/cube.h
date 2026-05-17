#include "face.h"

class Cube : public Hittable {
public:
  Cube(const point3 &point);

  bool is_hit(const ray &r, vec3 &normal) const override;

private:
  void update(void) override;

  std::array<Face, 6> faces;
};
