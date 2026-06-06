#include "world.h"
#include "cube.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>

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
