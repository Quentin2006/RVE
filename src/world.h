#pragma once

#include "cube.h"
#include <fstream>
#include <vector>

class World {
public:
  World() {};
  World(const std::ifstream &ifs) { loadFromFile(ifs); }

  int add(Cube &to_add) {
    world.push_back(to_add);
    return id++;
  }

  void remove(uint id) {
    if (id < world.size()) {
      world[id] = Cube(VoxelData{MaterialType::NONE, color(0, 0, 0), 0, 1.5f,
                                 1.0f, vec3{0, 0, 0}});
    }
  }

  void updateRefractionIndex(uint id, float newRefractionIndex) {
    if (id < world.size()) {
      world[id].setRefractionIndex(newRefractionIndex);
    }
  }

  bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const;

  auto begin() const { return world.begin(); }
  auto end() const { return world.end(); }

  auto begin() { return world.begin(); }
  auto end() { return world.end(); }

private:
  bool loadFromFile(const std::ifstream &ifs);

  std::vector<Cube> world;

  uint id = 0;
};
