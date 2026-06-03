#include "app.h"
#include <ctime>
#include <getopt.h>

struct options {
  std::string world_file;
};

int parse_options(int argc, char *argv[], options *options);

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  options options;
  parse_options(argc, argv, &options);

  App app(options.world_file);
  app.run();
}

int parse_options(int argc, char *argv[], options *options) {

  if (options == NULL) {
    return -1;
  }

  int opt;
  while ((opt = getopt(argc, argv, "hw:")) != -1) {
    switch (opt) {
    case 'w':
      options->world_file.assign(optarg);
      break;
    case 'h':
    default:
      printf("Usage: %s [OPTIONS]\n"
             "\n"
             "Options:\n"
             "  -w <file>    Path to world file\n"
             "  -h           Display this help message and exit\n"
             "\n"
             "Example:\n"
             "  %s -w world.txt\n",
             argv[0], argv[0]);
      exit(EXIT_SUCCESS);

      return -1;
    }
  }

  return 0;
}
