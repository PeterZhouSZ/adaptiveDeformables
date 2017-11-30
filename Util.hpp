#pragma once

#include <Eigen/dense>
#include <json/json.h>

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;


inline double poly6(double rSquared, double support){
  if(rSquared >= support*support){
	return 0;
  }

  return (315/(64*M_PI*std::pow(support, 9)))*std::pow(support*support - rSquared, 3);
  
}

template<typename T>
inline T getOrThrow(const Json::Value& jv, const std::string& field);

template<>
inline double getOrThrow<double>(const Json::Value& jv, const std::string& field){
  if(!jv.isMember(field)){
	throw std::runtime_error(field);
  }
  return jv[field].asDouble();
}

template<>
inline int getOrThrow<int>(const Json::Value& jv, const std::string& field){
  if(!jv.isMember(field)){
	throw std::runtime_error(field);
  }
  return jv[field].asInt();
}
