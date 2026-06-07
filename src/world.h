#pragma once

#include "cube.h"
#include <fstream>
#include <vector>

class World {
public:
  World() {};
  World(const std::string &file) { loadFromFile(file); }

  uint add(Cube &&to_add) {
    world.push_back(to_add);
    return id++;
  }

  void remove(uint objectId) {
    if (objectId < world.size()) {
      world[objectId] = Cube(VoxelData{MaterialType::NONE, color(0, 0, 0), 0, 1.5f,
                                       1.0f, vec3{0, 0, 0}});
    }
  }

  void updateRefractionIndex(uint objectId, float newRefractionIndex) {
    if (objectId < world.size()) {
      world[objectId].setRefractionIndex(newRefractionIndex);
    }
  }

  bool hit(const ray &r, float ray_min, float ray_max, HitRecord &rec) const;

  auto begin() const { return world.begin(); }
  auto end() const { return world.end(); }

  auto begin() { return world.begin(); }
  auto end() { return world.end(); }

private:
  bool loadFromFile(const std::string &string);

  std::vector<Cube> world;

  uint id = 0;
};
