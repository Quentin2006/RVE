#include "window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int _width, int _height, const std::string &_title)
    : window(
          glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr)),
      width(_width), height(_height) {

  glfwInit();

  glfwSetWindowSize(window, _width, _height);

  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << '\n';
    glfwTerminate();
  } else {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
  }
}
