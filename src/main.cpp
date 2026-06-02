#include "app.h"
#include <ctime>

int main() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  App app;
  app.run();
}
