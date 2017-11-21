#pragma once

#include <vector>
#include <json/json.h>
#include "Particle.hpp"

struct DeformableObject{

  DeformableObject(const Json::Value& jv);

  //initialization stuff
  void computeNeighbors();
  void computeBasisAndVolume();

  //timestepping methods
  void applyGravity(double dt);
  void applyElasticForces(double dt);
  void updatePositions(double dt);
  


  //data
  std::vector<Particle> particles;
  double lambda, mu, density;
  //static constants confuse me
  int desiredNumNeighbors() const { return 16; } 

private:
  //use to accumulate forces.  Stored as a member to avoid allocationg it each timestep
  std::vector<Vec3> forces;
  
  
};


//return the points corresponding to cluster centers
//from among those specified in indices
//n is the number of clusters
std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n);
