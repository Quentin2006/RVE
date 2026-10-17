#include "app.h"

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <limits>
#include <optional>
#include <sstream>
#include <string>

namespace {

struct Options {
  std::optional<std::string> world_file;
  SceneConfig scene;
  unsigned int seed = 42;
  glm::ivec3 size{10, 10, 10};
};

void print_usage(const char *program) {
  printf("Usage: %s [OPTIONS]\n"
         "\n"
         "Options:\n"
         "  -w, --world <file>     Path to a world file (skips generation)\n"
         "  -s, --seed <N>         Seed used to generate a world\n"
         "      --size <WxHxD>     Dimensions of the generated world\n"
         "  -c, --count <N>        Generate exactly N cubes (overrides --density)\n"
         "      --material <NAME>  lambertian, metal, dielectric, emissive or random\n"
         "      --density <MODE>   full or sparse\n"
         "  -h, --help             Display this help message and exit\n"
         "\n"
         "Example:\n"
         "  %s --seed 42 --size 10x10x10 --material random --density sparse\n",
         program, program);
}

bool parse_size(std::string_view text, glm::ivec3 &size) {
  std::istringstream stream{std::string(text)};
  int dimensions[3]{};

  for (int axis = 0; axis < 3; ++axis) {
    if (!(stream >> dimensions[axis])) {
      return false;
    }
    if (axis != 2) {
      char separator = '\0';
      if (!(stream >> separator) || separator != 'x') {
        return false;
      }
    }
  }

  if (dimensions[0] <= 0 || dimensions[1] <= 0 || dimensions[2] <= 0) {
    return false;
  }

  size = {dimensions[0], dimensions[1], dimensions[2]};
  return true;
}

std::optional<SceneMaterial> parse_material(const std::string &name) {
  if (name == "lambertian") {
    return SceneMaterial::LAMBERTIAN;
  }
  if (name == "metal") {
    return SceneMaterial::METAL;
  }
  if (name == "dielectric") {
    return SceneMaterial::DIELECTRIC;
  }
  if (name == "emissive") {
    return SceneMaterial::EMISSIVE;
  }
  if (name == "random") {
    return SceneMaterial::RANDOM;
  }
  return std::nullopt;
}

bool parse_density(const std::string &name, bool &sparse) {
  if (name == "full") {
    sparse = false;
    return true;
  }
  if (name == "sparse") {
    sparse = true;
    return true;
  }
  return false;
}

} // namespace

int main(int argc, char *argv[]) {
  Options options;

  static const option long_options[] = {
      {"world", required_argument, nullptr, 'w'},
      {"seed", required_argument, nullptr, 's'},
      {"size", required_argument, nullptr, 'z'},
      {"count", required_argument, nullptr, 'c'},
      {"material", required_argument, nullptr, 'm'},
      {"density", required_argument, nullptr, 'd'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, 0, nullptr, 0},
  };

  int opt;
  while ((opt = getopt_long(argc, argv, "w:s:z:m:d:hc", long_options,
                            nullptr)) != -1) {
    switch (opt) {
    case 'w':
      options.world_file = optarg;
      break;
    case 's': {
      char *end = nullptr;
      const unsigned long seed = std::strtoul(optarg, &end, 10);
      if (end == optarg || *end != '\0' ||
          seed > std::numeric_limits<unsigned int>::max()) {
        fprintf(stderr, "Error: --seed expects an unsigned integer, got '%s'\n",
                optarg);
        exit(EXIT_FAILURE);
      }
      options.seed = static_cast<unsigned int>(seed);
      break;
    }
    case 'c': {
      char *end = nullptr;
      const unsigned long count = std::strtoul(optarg, &end, 10);
      if (end == optarg || *end != '\0' ||
          count > std::numeric_limits<unsigned int>::max()) {
        fprintf(stderr, "Error: --count expects an integer, got '%s'\n",
                optarg);
        exit(EXIT_FAILURE);
      }
      options.scene.count = static_cast<unsigned int>(count);
      break;
    }
    case 'z':
      if (!parse_size(optarg, options.size)) {
        fprintf(stderr, "Error: --size expects WxHxD (e.g. 10x10x10), got '%s'\n",
                optarg);
        exit(EXIT_FAILURE);
      }
      break;
    case 'm': {
      const auto material = parse_material(optarg);
      if (!material) {
        fprintf(stderr, "Error: unknown material '%s'\n", optarg);
        exit(EXIT_FAILURE);
      }
      options.scene.material = *material;
      break;
    }
    case 'd':
      if (!parse_density(optarg, options.scene.sparse)) {
        fprintf(stderr,
                "Error: unknown density '%s' (expected full or sparse)\n",
                optarg);
        exit(EXIT_FAILURE);
      }
      break;
    case 'h':
      print_usage(argv[0]);
      exit(EXIT_SUCCESS);
    default:
      print_usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind < argc) {
    fprintf(stderr, "Error: unexpected argument '%s'\n", argv[optind]);
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (options.world_file && options.scene.count) {
    fprintf(stderr, "Error: --count only applies to generated worlds, not "
                    "--world files\n");
    exit(EXIT_FAILURE);
  }

  if (options.scene.count && !options.world_file) {
    const unsigned long long capacity =
        static_cast<unsigned long long>(options.size.x) *
        static_cast<unsigned long long>(options.size.y) *
        static_cast<unsigned long long>(options.size.z);
    if (*options.scene.count > capacity) {
      fprintf(stderr,
              "Error: --count %u exceeds the %dx%dx%d grid, "
              "which has %llu slots; increase --size\n",
              *options.scene.count, options.size.x, options.size.y,
              options.size.z,
              static_cast<unsigned long long>(capacity));
      exit(EXIT_FAILURE);
    }
  }

  if (options.world_file) {
    App app(*options.world_file);
    app.run();
  } else {
    App app(options.seed, options.size, options.scene);
    app.run();
  }
}
