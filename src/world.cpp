#include "world.h"
#include "cube.h"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>

void World::generate(unsigned int seed, glm::ivec3 dims,
                     const SceneConfig &config) {
  std::mt19937 rng{seed};

  // One entry per selectable material. NONE marks a voxel that simply keeps
  // its gap, like the --none option in gen_scene.py.
  struct PoolEntry {
    MaterialType mat{MaterialType::NONE};
    glm::vec2 param_range{0.f, 0.f}; // min/max of the material specific parameter
  };

  std::vector<PoolEntry> pool;
  const auto add_material = [&pool](MaterialType mat, glm::vec2 param_range) {
    pool.push_back({mat, param_range});
  };

  switch (config.material) {
  case SceneMaterial::LAMBERTIAN:
    add_material(MaterialType::LAMBERTIAN, {0.f, 0.f});
    break;
  case SceneMaterial::METAL:
    add_material(MaterialType::METAL, config.fuzz_range);
    break;
  case SceneMaterial::DIELECTRIC:
    add_material(MaterialType::DIELECTRIC, config.refraction_range);
    break;
  case SceneMaterial::EMISSIVE:
    add_material(MaterialType::EMISSIVE, config.brightness_range);
    break;
  case SceneMaterial::RANDOM:
    add_material(MaterialType::LAMBERTIAN, {0.f, 0.f});
    add_material(MaterialType::METAL, config.fuzz_range);
    add_material(MaterialType::DIELECTRIC, config.refraction_range);
    add_material(MaterialType::EMISSIVE, config.brightness_range);
    break;
  }

  // In count mode every placed cube is a real material, the sparse gap
  // pool only makes sense for the whole-grid fill mode.
  if (config.sparse && !config.count) {
    add_material(MaterialType::NONE, {0.f, 0.f});
  }

  if (pool.empty()) {
    buildBvh();
    return;
  }

  std::uniform_int_distribution<size_t> pick_material{0, pool.size() - 1};
  std::uniform_real_distribution<float> unit{0.f, 1.f};

  const auto make_cube = [&](const PoolEntry &entry, int x, int y, int z) {
    VoxelData lineData;
    lineData.mat = entry.mat;

    const float r = unit(rng);
    const float g = unit(rng);
    const float b = unit(rng);
    lineData.albedo = {r, g, b};

    // Materials with an extra parameter pick it like gen_scene.py does:
    // uniformly from the parameter range, in hundredths.
    if (entry.mat != MaterialType::LAMBERTIAN) {
      std::uniform_int_distribution<int> pick_param{
          static_cast<int>(entry.param_range.x * 100.f),
          static_cast<int>(entry.param_range.y * 100.f)};
      const float param =
          static_cast<float>(pick_param(rng)) / 100.f;

      switch (entry.mat) {
      case MaterialType::METAL:
        lineData.fuzz = param;
        break;
      case MaterialType::DIELECTRIC:
        lineData.refraction_index = param;
        break;
      case MaterialType::EMISSIVE:
        lineData.brightness = param;
        break;
      default:
        break;
      }
    }

    lineData.pos = {static_cast<float>(x), static_cast<float>(y),
                    static_cast<float>(z)};
    return Cube(lineData);
  };

  if (config.count) {
    const size_t capacity = static_cast<size_t>(dims.x) *
                            static_cast<size_t>(dims.y) *
                            static_cast<size_t>(dims.z);
    if (capacity == 0) {
      buildBvh();
      return;
    }

    const size_t count = std::min<size_t>(*config.count, capacity);
    if (count < *config.count) {
      std::cerr << "Error: requested " << *config.count
                << " cubes but the grid only has " << capacity
                << " slots, increase --size\n";
      buildBvh();
      return;
    }

    // Choose count unique slots with a partial Fisher-Yates shuffle and
    // decode the linear slot index back into (x, y, z), same layout the
    // fill mode below iterates in.
    std::vector<size_t> slots(capacity);
    std::iota(slots.begin(), slots.end(), 0);

    const size_t stride_x = static_cast<size_t>(dims.x);
    const size_t stride_y = stride_x * static_cast<size_t>(dims.y);

    for (size_t placed = 0; placed < count; ++placed) {
      std::uniform_int_distribution<size_t> pick_slot{placed, capacity - 1};
      std::swap(slots[placed], slots[pick_slot(rng)]);

      const size_t index = slots[placed];
      const int x = static_cast<int>(index % stride_x);
      const int y =
          static_cast<int>((index / stride_x) % static_cast<size_t>(dims.y));
      const int z = static_cast<int>(index / stride_y);

      add(make_cube(pool[pick_material(rng)], x, y, z));
    }
    buildBvh();
    return;
  }

  for (int z = 0; z < dims.z; ++z) {
    for (int y = 0; y < dims.y; ++y) {
      for (int x = 0; x < dims.x; ++x) {
        const PoolEntry &entry = pool[pick_material(rng)];
        if (entry.mat == MaterialType::NONE) {
          continue;
        }

        add(make_cube(entry, x, y, z));
      }
    }
  }
  buildBvh();
}

bool World::hit(const ray &r, float ray_min, float ray_max,
                HitRecord &rec) const {
  if (!bvh_dirty && !bvh.requiresLinear(r)) {
    // A caller may retain a mutable iterator across buildBvh(), then use it
    // to change a cube without calling begin()/end() again.
    bool unchanged = true;
    if (mutable_iterators_exposed) {
      if (bvh_transforms.size() != world.size()) {
        unchanged = false;
      } else {
        for (size_t i = 0; i < world.size(); ++i) {
          if (bvh_transforms[i] != world[i].modelMatrixRef()) {
            unchanged = false;
            break;
          }
        }
      }
    }
    if (unchanged) return bvh.hit(world, r, ray_min, ray_max, rec);
  }
  return hit_linear(r, ray_min, ray_max, rec);
}

void World::buildBvh() {
  bvh.build(world);
  bvh_transforms.clear();
  if (mutable_iterators_exposed) {
    bvh_transforms.reserve(world.size());
    for (const Cube &cube : world) bvh_transforms.push_back(cube.modelMatrixRef());
  }
  bvh_dirty = false;
}

bool World::hit_linear(const ray &r, float ray_min, float ray_max,
                       HitRecord &rec) const {
  bool hit_anything = false;
  float closest_t = ray_max;

  for (const auto &object : world) {
    HitRecord candidate_rec;
    if (object.hit(r, ray_min, closest_t, candidate_rec) &&
        candidate_rec.t < closest_t) {
      hit_anything = true;
      closest_t = candidate_rec.t;
      rec = candidate_rec;
    }
  }

  return hit_anything;
}

bool World::loadFromFile(const std::string &filename) {
  // we will fill in the VoxelData struct from the file and then construct Cubes
  // from that data and add them to the world

  // struct VoxelData {
  //   MaterialType mat;
  //   color albedo;
  //
  //   float fuzz{0.f};
  //   float refraction_index{1.5f};
  //   float brightness{1.f};
  //
  //   vec3 pos;
  // };

  // will be in the following format:
  //
  // x y z 0(LAMBERTIAN)|1(METAL)|2(DIELECTRIC)|3(EMISSIVE) r g b
  // fuzz|refraction_index|brightness

  if (filename.empty()) {
    buildBvh();
    return false;
  }

  std::ifstream inputFile(filename);
  std::string line;

  int lineNum = 1;
  while (std::getline(inputFile, line)) {
    std::vector<std::string> v;
    boost::split(v, line, boost::is_any_of(" "));

    if (v.size() >= 1 && (v[0] == "//" || v[0].empty() || v[0] == " ")) {
      continue; // skip commnets or empty lines
    }

    if (v.size() != 8 && (v[3] == "0" && v.size() != 7)) {
      std::cerr << "Error parsing world file: line " << lineNum
                << " does not have correct # of components" << std::endl;
      continue;
    }

    // populate the VoxelData struct from the line
    VoxelData lineData;

    lineData.pos = vec3(std::stof(v[0]), std::stof(v[1]), std::stof(v[2]));
    lineData.mat = static_cast<MaterialType>(std::stoi(v[3]));
    lineData.albedo = color(std::stof(v[4]), std::stof(v[5]), std::stof(v[6]));

    switch (lineData.mat) {
    case LAMBERTIAN:
      break;
    case METAL:
      lineData.fuzz = std::stof(v[7]);
      break;
    case DIELECTRIC:
      lineData.refraction_index = std::stof(v[7]);
      break;
    case EMISSIVE:
      lineData.brightness = std::stof(v[7]);
      break;
    case NONE:
      break;
    }

    ++lineNum;

    World::add(Cube(lineData));
  }

  buildBvh();
  return true;
}
