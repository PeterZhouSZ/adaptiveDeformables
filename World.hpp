#pragma once

#include <vector>
#include <string_view>
#include "DeformableObject.hpp"


class World{

public:

  World(std::string_view filename);

  void step();
  void dump();

  double elapsedTime, duration;
  double dt;
  std::vector<DeformableObject> dos;

  int frameNumber;
};
