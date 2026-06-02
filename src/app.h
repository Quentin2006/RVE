#pragma once

#include "camera.h"
#include "window.h"
#include "world.h"
#include <array>
#include <chrono>
#include <iostream>

class App {
public:
  App() : world() {};
  App(const std::ifstream &ifp) : world(ifp) {};
  void run();

private:
  Camera camera{window::WIDTH, window::HEIGHT};
  World world;
  Window window{window::WIDTH, window::HEIGHT};
};
