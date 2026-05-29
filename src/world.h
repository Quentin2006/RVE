#pragma once

#include "hittable/hittable.h"
#include <memory>
#include <vector>

class World {
public:
  int add(std::unique_ptr<Hittable> to_add) {
    world.push_back(std::move(to_add));
    return id++;
  }

  void remove(uint id) { world[id] = nullptr; }

  bool hit(const ray &r, HitRecord &rec, float t_max) const;

  auto begin() const { return world.begin(); }
  auto end() const { return world.end(); }

  auto begin() { return world.begin(); }
  auto end() { return world.end(); }

private:
  std::vector<std::unique_ptr<Hittable>> world;

  uint id = 0;
};
