#pragma once

#include <vector>
#include <string_view>
#include "DeformableObject.hpp"
#include "Stopwatch.hpp"

class World{

public:

  World(std::string_view filename);

  void step();

  void stepNoOvershoot();
  
  void dump();

  void dumpMitsuba();
  
  double elapsedTime, duration;
  double dt;
  std::vector<DeformableObject> dos;

  int frameNumber;


  double simTime;

  std::string mitsubaInfo;
};
