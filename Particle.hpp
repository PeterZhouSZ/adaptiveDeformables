#pragma once

#include "Util.hpp"

struct Particle{
  Vec3 position;
  Vec3 velocity;
  std::vector<std::pair<int, Vec3> > neighbors;
  
};
