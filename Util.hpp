#pragma once

#include <Eigen/dense>

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;


inline double poly6(double rSquared, double support){
  if(rSquared >= support*support){
	return 0;
  }

  return (315/(64*M_PI*std::pow(support, 9)))*std::pow(support*support - rSquared, 3);
  
}
