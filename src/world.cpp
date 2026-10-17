#include "world.h"
#include "cube.h"
#include <boost/algorithm/string.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
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

  if (config.sparse) {
    add_material(MaterialType::NONE, {0.f, 0.f});
  }

  if (pool.empty()) {
    return;
  }

  std::uniform_int_distribution<size_t> pick_material{0, pool.size() - 1};
  std::uniform_real_distribution<float> unit{0.f, 1.f};

  for (int z = 0; z < dims.z; ++z) {
    for (int y = 0; y < dims.y; ++y) {
      for (int x = 0; x < dims.x; ++x) {
        const PoolEntry &entry = pool[pick_material(rng)];
        if (entry.mat == MaterialType::NONE) {
          continue;
        }

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

        World::add(Cube(lineData));
      }
    }
  }
}

bool World::hit(const ray &r, float ray_min, float ray_max,
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

  return true;
}
