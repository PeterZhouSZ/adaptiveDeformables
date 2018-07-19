#pragma once

#include <vector>
#include <iostream>
#include <json/json.h>
#include "Particle.hpp"

struct DeformableObject{

  DeformableObject(const Json::Value& jv);

  void dump(const std::string& filename) const;

  void dumpWithColor(const std::string& filename) const;
  
  //void writeHierarchy(const std::string& filename) const;
  
  //initialization stuff
  void computeNeighbors();
  void computeBasisAndVolume();
  //void computeHierarchy();

  
  //timestepping methods
  void applyGravity(double dt);
  void applyElasticForces(double dt);
  //void applyElasticForcesAdaptive(double dt);
  void applyElasticForcesNoOvershoot(double dt);

  void updatePositions(double dt);
  void bounceOffGround();
  void damp(double dt);
  void springDamping(double dt);

  
  Mat3 computeDeformationGradient(int pIndex) const;
  Mat3 computeDeformationGradientRBF(int pIndex) const;


  void RBFInit();

  
  //data
  std::vector<Particle> particles;
  double lambda, mu, density, dampingFactor;
  double rbfDelta;
  //double scalingVarianceThreshold, angularVarianceThreshold;
  //int hierarchyLevels, parentsPerParticle,
  int neighborsPerParticle;

  double particleSize;
  
  
  //static constants confuse me
  //int desiredNumNeighbors() const { return 24; } 

  std::vector<RenderInfo> renderInfos;
  

  void assertFinite() const{
	for(const auto& p : particles){
	  if(!p.position.allFinite()){
		std::cout << "bad position! " << std::endl;
		exit(1);
	  }
	  if(!p.velocity.allFinite()){
		std::cout << "bad velocity!" << std::endl;
		exit(1);
	  }
	}
	std::cout << "all finite!" << std::endl;
  }
  
private:
  //use to accumulate forces.  Stored as a member to avoid allocationg it each timestep
  std::vector<Vec3> forces;
  std::vector<Vec3> dampedVelocities; //same for damping


  //hierarchy 0 is the smallest
  //std::vector<std::vector<int>> hierarchy;

  
};


//return the points corresponding to cluster centers
//from among those specified in indices
//n is the number of clusters
//returned vector IS SORTED
std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n);
