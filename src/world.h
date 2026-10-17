#pragma once

#include "cube.h"
#include <fstream>
#include <optional>
#include <vector>

enum class SceneMaterial { LAMBERTIAN, METAL, DIELECTRIC, EMISSIVE, RANDOM };

// Configuration for procedurally generated benchmark worlds.
struct SceneConfig {
  SceneMaterial material{SceneMaterial::RANDOM};
  bool sparse{false};

  // Ranges for each material's extra parameter. Defaults match the
  // values gen_scene.py used for the committed benchmark scenes:
  // fuzz 0.0, refraction_index 1.5, brightness 1.0.
  glm::vec2 fuzz_range{0.0f, 0.0f};
  glm::vec2 refraction_range{1.5f, 1.5f};
  glm::vec2 brightness_range{1.0f, 1.0f};

  // Generate exactly this many cubes in randomly chosen grid slots.
  // When set, every placed cube is a real material (the sparse gap pool
  // is skipped) and --density has no effect.
  std::optional<unsigned int> count;
};

class World {
public:
  World() {};
  World(const std::string &file) { loadFromFile(file); }

  // Generates a deterministic world from a seed, filling the given
  // dimensions with voxels of the configured material(s).
  void generate(unsigned int seed, glm::ivec3 dims, const SceneConfig &config);

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
