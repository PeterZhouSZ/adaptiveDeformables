#pragma once

#include <vector>
#include <json/json.h>
#include "Particle.hpp"

struct DeformableObject{

  DeformableObject(const Json::Value& jv);

  void computeNeighbors();

  void applyGravity(double dt);
  void applyElasticForces(double dt);
  void updatePositions(double dt);
  
  std::vector<Particle> particles;

  int desiredNumNeighbors() const { return 32; }
  
};


//return the points corresponding to cluster centers
//from among those specified in indices
//n is the number of clusters
std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n);
