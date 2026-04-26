#pragma once

#include "cube.h"
#include <memory>
#include <vector>

class World {
public:
  void add(std::unique_ptr<Cube> to_add) { world.push_back(std::move(to_add)); }

  auto begin() const { return world.begin(); }
  auto end() const { return world.end(); }

  auto begin() { return world.begin(); }
  auto end() { return world.end(); }

private:
  std::vector<std::unique_ptr<Cube>> world;
};
