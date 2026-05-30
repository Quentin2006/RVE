#include "camera.h"
#include "consts.h"
#include "hittable/cube.h"
#include "hittable/material.h"
#include "window.h"
#include "world.h"
#include <array>
#include <chrono>
#include <iostream>
#include <vector>

class App {
public:
  App() {}

  void run();

private:
  Camera camera{window::WIDTH, window::HEIGHT};
  World world{};
  Window window{window::WIDTH, window::HEIGHT};
};
