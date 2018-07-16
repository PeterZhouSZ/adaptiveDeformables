#pragma once

#include "IndexedHeap.hpp"
#include "KDTree.hpp"
#include <Eigen/Dense>

inline double weightFunction(double distance, double rMin, double rMax){
  double adjustedDistance = distance < 2*rMin ? 2*rMin : std::min(distance, 2*rMax);
  return std::pow(1 - adjustedDistance/(2*rMax), 8);
}

template<typename Point>
std::vector<int> sampleEliminate(const std::vector<Point>& points, const std::vector<int>& legalIndices, int M){
  
  std::vector<double> weights(points.size(), 0);
  
  Point minBounds = points[legalIndices[0]];
  Point maxBounds = points[legalIndices[0]];
  for(auto i : legalIndices){
	minBounds.array() = minBounds.array().min(points[i].array());
	maxBounds.array() = maxBounds.array().max(points[i].array());
  }
  std::cout << "minBounds: " << minBounds << " maxBounds: " << maxBounds << std::endl;
  double volume = (maxBounds - minBounds).prod();
  double rMax = std::cbrt(volume/(4*std::sqrt(2)*points.size()));
  double rMin = 0.65*rMax*( 1 - std::pow(static_cast<double>(points.size())/M, 1.5));


  
  KDTree<Point,3> kdTree(points, legalIndices);
  
  for(auto i : legalIndices){
	auto neighbors = kdTree.rangeQuery(points, points[i], 2*rMax);
	for(auto n : neighbors){
	  if(n != i){
		weights[i] += weightFunction( (points[i] - points[n]).norm(), rMin, rMax);
	  }
	}
  }
	  
  IndexedHeap ih(weights, legalIndices);
  while(ih.size() > M){
	int popped = ih.popMax();
	auto neighbors = kdTree.rangeQuery(points, points[popped], 2*rMax);
	for(auto n : neighbors){
	  if(n != popped){
		weights[n] -= weightFunction((points[popped] - points[n]).norm(), rMin, rMax);
		ih.elementReweighted(n);
	  }
	}
  }

  return std::move(ih).extractVector() ;
}
