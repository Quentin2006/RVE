#include "../src/world.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>

namespace {
std::uint64_t total = 0;
std::uint64_t mismatches = 0;

void compare(const World &world, const ray &r, float ray_min, float ray_max,
             const std::string &name) {
  HitRecord accelerated;
  HitRecord linear;
  const bool a = world.hit(r, ray_min, ray_max, accelerated);
  const bool b = world.hit_linear(r, ray_min, ray_max, linear);
  ++total;
  if (a != b || (a && (accelerated.hit_object != linear.hit_object ||
                       std::abs(accelerated.t - linear.t) > 1e-4f))) {
    ++mismatches;
    if (mismatches <= 10) {
      std::cerr << "Mismatch in " << name << " ray " << total
                << ": hit=" << a << "/" << b << " t=" << accelerated.t
                << "/" << linear.t << '\n';
    }
  }
}

void exercise(const World &world, unsigned int seed, int count, float size,
              const std::string &name) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<float> inside(0.0f, size);
  std::uniform_real_distribution<float> outside(-size, 2.0f * size);
  std::uniform_real_distribution<float> direction(-1.0f, 1.0f);
  for (int i = 0; i < count; ++i) {
    // Half the origins lie inside the grid; the others are guaranteed outside.
    vec3 origin(inside(rng), inside(rng), inside(rng));
    if (i % 2 != 0) {
      origin = vec3(outside(rng), outside(rng), outside(rng));
      origin[i % 3] = (i % 4 == 1) ? -inside(rng) - 0.01f
                                    : size + inside(rng) + 0.01f;
    }
    vec3 dir(direction(rng), direction(rng), direction(rng));
    // Include axis-parallel rays as well as rays aimed back at the grid.
    if (i % 5 == 0) dir[i % 3] = 0.0f;
    if (i % 5 == 1) {
      dir = vec3(inside(rng), inside(rng), inside(rng)) - origin;
    }
    compare(world, ray(origin, dir), 0.001f,
            std::numeric_limits<float>::infinity(), name);
  }
}

Cube cube_at(vec3 position) {
  return Cube(VoxelData{MaterialType::LAMBERTIAN, color(0.4f, 0.5f, 0.6f),
                        0.0f, 1.5f, 1.0f, position});
}
} // namespace

int main() {
  const SceneMaterial materials[] = {SceneMaterial::LAMBERTIAN,
                                     SceneMaterial::DIELECTRIC,
                                     SceneMaterial::RANDOM};
  unsigned int ray_seed = 1000;
  for (unsigned int seed = 1; seed <= 5; ++seed) {
    for (SceneMaterial material : materials) {
      for (bool sparse : {false, true}) {
        for (int size : {5, 10}) {
          World world;
          SceneConfig config;
          config.material = material;
          config.sparse = sparse;
          world.generate(seed, glm::ivec3(size), config);
          exercise(world, ray_seed++, 10000, static_cast<float>(size),
                   "generated grid");
        }
      }
    }
  }
  std::cout << "Generated grids: " << total << " rays\n";

  World count_world;
  SceneConfig count_config;
  count_config.count = 50;
  count_world.generate(42, glm::ivec3(10), count_config);
  exercise(count_world, ray_seed++, 2000, 10.0f, "count");

  // Test each benchmark configuration, excluding the out-of-scope 50^3 runs.
  for (const std::string material : {"lambertian", "metal", "dielectric",
                                     "emissive", "random"}) {
    for (const std::string density : {"full", "sparse"}) {
      for (int size : {5, 10}) {
        const std::string dimension = std::to_string(size);
        const std::string path = "benchmark/scenes/" + material + "/" +
            density + "/" + dimension + "x" + dimension + "x" + dimension +
            ".scene";
        World world(path);
        exercise(world, ray_seed++, 2000, static_cast<float>(size), path);
      }
    }
  }

  World world;
  for (int i = 0; i < 12; ++i) {
    world.add(cube_at(vec3(static_cast<float>(i % 4) * 2.0f,
                           static_cast<float>(i / 4) * 2.0f, 2.0f)));
  }
  int i = 0;
  for (Cube &cube : world) {
    cube.setRotation(vec3(0.13f * static_cast<float>(i),
                          0.2f, -0.15f));
    cube.setScale(vec3(0.5f + 0.05f * static_cast<float>(i), 1.5f, -0.8f));
    cube.move(cube.getVoxelData().pos + vec3(0.3f, 0.2f, 0.4f));
    ++i;
  }
  world.buildBvh();
  exercise(world, ray_seed++, 2000, 10.0f, "transforms");

  // Duplicate boxes exercise equal-t identity and the original index order.
  world.add(cube_at(vec3(0.0f)));
  world.add(cube_at(vec3(0.0f)));
  exercise(world, ray_seed++, 2000, 10.0f, "add before rebuild");
  world.buildBvh();
  exercise(world, ray_seed++, 2000, 10.0f, "add after rebuild");
  world.remove(0);
  exercise(world, ray_seed++, 2000, 10.0f, "remove before rebuild");
  world.buildBvh();
  exercise(world, ray_seed++, 2000, 10.0f, "remove after rebuild");

  // An iterator obtained after a build permits mutation and must dirty it.
  world.begin()->move(vec3(8.0f, 8.0f, 8.0f));
  exercise(world, ray_seed++, 2000, 10.0f, "move before rebuild");
  world.buildBvh();
  exercise(world, ray_seed++, 2000, 10.0f, "move after rebuild");

  // Test a tie across separate leaves, including the original first cube.
  World ties;
  for (int box = 0; box < 8; ++box) ties.add(cube_at(vec3(0.0f)));
  ties.buildBvh();
  compare(ties, ray(vec3(-2.0f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f)),
          0.001f, std::numeric_limits<float>::infinity(), "equal-t tie");

  // Cube::hit treats sufficiently small local direction components as
  // parallel, even for a very distant intersection on a different axis.
  World near_parallel;
  near_parallel.add(cube_at(vec3(0.0f)));
  near_parallel.buildBvh();
  compare(near_parallel,
          ray(vec3(0.5f, -10000000.0f, 0.5f), vec3(0.0000005f, 1.0f, 0.0f)),
          0.001f, std::numeric_limits<float>::infinity(), "near-parallel");

  std::cout << "Total rays: " << total << "\n"
            << "Mismatches: " << mismatches << '\n';
  return mismatches == 0 ? 0 : 1;
}
