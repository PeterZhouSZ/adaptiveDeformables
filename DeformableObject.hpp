#pragma once

#include <vector>
#include <json/json.h>
#include "Particle.hpp"

struct DeformableObject{

  DeformableObject(const Json::Value& jv);

  void dump(const std::string& filename) const;
  
  //initialization stuff
  void computeNeighbors();
  void computeBasisAndVolume();
  void computeHierarchy();

  
  //timestepping methods
  void applyGravity(double dt);
  void applyElasticForces(double dt);
  void updatePositions(double dt);
  void bounceOffGround();
  void damp(double dt);


  //data
  std::vector<Particle> particles;
  double lambda, mu, density, dampingFactor;
  int hierarchyLevels;
  //static constants confuse me
  int desiredNumNeighbors() const { return 24; } 


  void assertFinite() const{
	for(const auto& p : particles){
	  assert(p.position.allFinite());
	  assert(p.velocity.allFinite());
	}
  }
  
private:
  //use to accumulate forces.  Stored as a member to avoid allocationg it each timestep
  std::vector<Vec3> forces;
  std::vector<Vec3> dampedVelocities; //same for damping


  //hierarchy 0 is the smallest
  std::vector<std::vector<int>> hierarchy;

  
};


//return the points corresponding to cluster centers
//from among those specified in indices
//n is the number of clusters
//returned vector IS SORTED
std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n);
