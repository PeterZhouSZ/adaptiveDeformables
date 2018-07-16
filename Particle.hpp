#pragma once

#include "Util.hpp"

struct Neighbor{
  int index;
  Vec3 uij;
  double wij;
};

struct Parent{
  int index;
  double wij;
};

struct RenderInfo{
  Vec3 color;
  double size;
};

struct Particle{
  Vec3 position;
  Vec3 velocity;

  Mat3 Ainv;
  double volume;
  double kernelRadius;

  QuatSVD::EigenSVD<double> svd; //make interpolation easier...
  std::vector<Neighbor> neighbors;
  //double wSum; //just normalize
  std::vector<Parent> parents;
};
