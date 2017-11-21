#pragma once

#include "Util.hpp"

struct Neighbor{
  int index;
  Vec3 uij;
  double wij;
};

struct Particle{
  Vec3 position;
  Vec3 velocity;

  Mat3 Ainv;
  double volume;
  double kernelRadius;
  
  std::vector<Neighbor> neighbors;
  
};
