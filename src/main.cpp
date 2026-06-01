#include "app.h"

#include <cstdlib>
#include <ctime>

int main() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  App app;
  app.run();

  return EXIT_SUCCESS;
}
