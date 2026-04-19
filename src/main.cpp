#include "window.h"

int main(int argc, char *argv[]) {
  Window window{800, 800, ""};

  while (!window.shouldClose()) {
    window.swapBuffers();
  }
  return 0;
}
