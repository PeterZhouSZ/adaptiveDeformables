#pragma once

#include <vector>
#include <string>
#include "Particle.hpp"

struct DeformableObject{

  DeformableObject(const std::string& filename);



  
  std::vector<Particle> particles;
  
};


//return the points corresponding to cluster centers
//from among those specified in indices
//n is the number of clusters
std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n);
