#pragma once

#include <vector>
#include "Util.hpp"


//Px(P) returns an Eigen::Vector3d with the position of P
template <typename ParticleType, typename PX>
class AccelerationGrid{

public:

  AccelerationGrid(int nb) : numBuckets(nb) {}
  

  
  std::vector<int> getNearestNeighbors(const std::vector<ParticleType>& particles, 
									   const Vec3& x, double radius) const;
  
  void updateGrid(const std::vector<ParticleType>& particles);

  template <typename RGetter>
  void updateGridWithRadii(const std::vector<ParticleType>& particles,  RGetter&& rGetter);

  //return the pairs that might be colliding
  std::vector<std::pair<int, int> > getPotentialPairs() const; 

  Vec3 getDelta() const { return delta; }
  
private:
  std::vector<std::vector<int>> grid;  
  Vec3 origin, delta;
  int numBuckets;
  
  Eigen::Vector3i inline getBucket(Vec3 position) const{
	return Eigen::Vector3i {((position - 
			  origin).array()/
		  delta.array()).template cast<int>()
		}.unaryExpr([this](int a){
			  return std::max(0,std::min(a, numBuckets -1));
			});
  }
  
  
  inline int index(int i, int j, int k) const {
	return i*numBuckets*numBuckets +
	  j*numBuckets + k;
  }
  
  inline int index(Eigen::Vector3i v) const {
	return index(v(0), v(1), v(2));
  }
  
};


//template madness
#include "AccelerationGrid.cpp"
