#pragma once

#include "cube.h"
#include <vector>

class World {
public:
  int add(Cube &&to_add) {
    world.push_back(std::move(to_add));
    return id++;
  }

  void remove(uint id) {
    if (id < world.size()) {
      world[id] = Cube(vec3(0, 0, 0), MaterialType::NONE, color(0, 0, 0));
    }
  }

  bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const;

  auto begin() const { return world.begin(); }
  auto end() const { return world.end(); }

  auto begin() { return world.begin(); }
  auto end() { return world.end(); }

private:
  std::vector<Cube> world;

  uint id = 0;
};
