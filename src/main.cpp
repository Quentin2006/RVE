#include "window.h"

int main() {
  Window window{800, 800, ""};

  while (!window.shouldClose()) {
    window.swapBuffers();
  }
  return 0;
}
