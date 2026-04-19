#include "window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int w, int h, const std::string &t) : width(w), height(h) {
  glfwInit();
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

  window = glfwCreateWindow(w, h, t.c_str(), NULL, NULL);
  glfwSetWindowSize(window, w, h);

  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  } else {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
  }
}
