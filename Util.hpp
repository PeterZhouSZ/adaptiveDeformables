#pragma once

#include <Eigen/dense>
#include <json/json.h>
#include "quatSVD.hpp"

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using Quat = Eigen::Quaterniond;

template<typename T>
T square(const T& t){
  return t*t;
}

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



template<typename QR, typename WR> //QR, WR something array-like
Quat interpolateQuats(QR&& quats, WR&& weights){

  auto len = quats.size();
  assert(quats.size == weights.size());

  auto pivot = quats[len/2];

  Quat ret{0,0,0,0};
  for(auto i = 0; i < len; ++i){
	ret += ((pivot.dot(quats[i]) < 0) ?
		-weights[i] : weights[i])*quats[i];
  }
  return ret.normalized();
  
}

template<typename VS, typename WS>
Vec3 interpolateVectors(VS&& vecs, WS&& weights){
  
  auto len = vecs.size();
  assert(len == weights.size());
  auto ret = Vec3::Zero();
  double wSum = 0;
  for(auto i = 0; i < len; ++i){
	ret += weights[i]*vecs[i];
	wSum += weights[i];
  }
  return ret/wSum;
}


/*
  Form polar decomp (RS), 
R = U*V.transpose
S = V Sigma V.transpose


 */
struct PolarDecomp{ Quat R; Mat3 S;};

template<typename SVDs, typename Ws>
PolarDecomp interpolateSVDsPolar(const SVDs& svds, const Ws& weights){
  auto len = svds.size();
  assert(len == weights.size());
  auto pivotU = svds[len/2].U;
  auto pivotV = svds[len/2].V;

  Quat pivotR = pivotU*pivotV.conjugate();

  
  double wSum = 0;

  Quat R(0,0,0,0);
  Mat3 S = Mat3::Zero();
  
  for(auto i = 0; i < len; ++i){

	Quat thisR = svds[i].U*svds[i].V.conjugate();
	Mat3 thisS = svds[i].V.toRotationMatrix()*svds[i].S.asDiagonal()*svds[i].V.conjugate().toRotationMatrix();

	R.coeffs() += (pivotR.dot(thisR) < 0 ? -weights[i] : weights[i])*thisR.coeffs();
	S += weights[i]*thisS;
	wSum += weights[i];
	
  }
  R.normalize();
  S /= wSum;

  return {R, S};
}


template<typename SVDs, typename Ws>
QuatSVD::EigenSVD<double> interpolateSVDs(const SVDs& svds, const Ws& weights){
  auto len = svds.size();
  assert(len == weights.size());

  
  
  auto pivotU = svds[len/2].U;
  auto pivotV = svds[len/2].V;
  double wSum = 0;
  QuatSVD::EigenSVD<double> ret{ Vec3(0,0,0), Quat(0,0,0,0), Quat(0,0,0,0) };
  for(auto i = 0; i < len; ++i){
	ret.U.coeffs() += (pivotU.dot(svds[i].U) < 0 ? -weights[i] : weights[i])*svds[i].U.coeffs();
	ret.V.coeffs() += (pivotV.dot(svds[i].V) < 0 ? -weights[i] : weights[i])*svds[i].V.coeffs();
	ret.S += weights[i]*svds[i].S;
	wSum += weights[i];
	
  }
  ret.U.normalize();
  ret.V.normalize();
  ret.S /= wSum;

  return ret;
}







inline std::ostream& operator<<(std::ostream& outs, const Quat& q){
  outs << q.w() << ' ' << q.x() << ' ' << q.y() << ' ' << q.z();
  return outs;
}

template<typename T>
std::ostream& operator<<(std::ostream& outs, const QuatSVD::EigenSVD<T>& svd){
  outs << "SVD{ U{ " << svd.U << " } V{ "<< svd.V << " } S { " << svd.S << " } }";
  return outs;
}

inline Vec3 hsv2rgb(double h, double s, double v)
{
    double hh, p, q, t, ff;
    long i;
    Vec3 out;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
	  return Vec3(v,v,v);
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
	  out.x() = v;
	  out.y() = t;
	  out.z() = p;
	  break;
    case 1:
	  out.x() = q;
	  out.y() = v;
	  out.z() = p;
	  break;
    case 2:
	  out.x() = p;
	  out.y() = v;
	  out.z() = t;
	  break;
	  
    case 3:
	  out.x() = p;
	  out.y() = q;
	  out.z() = v;
	  break;
    case 4:
	  out.x() = t;
	  out.y() = p;
	  out.z() = v;
	  break;
    case 5:
    default:
	  out.x() = v;
	  out.y() = p;
	  out.z() = q;
	  break;
    }
    return out;     
}



/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   {number}  h       The hue
 * @param   {number}  s       The saturation
 * @param   {number}  l       The lightness
 * @return  {Array}           The RGB representation
 */
/*
inline double hue2rgb(double p, double q, double t){
  if(t < 0) t += 1;
  if(t > 1) t -= 1;
  if(t < 1/6) return p + (q - p) * 6 * t;
  if(t < 1/2) return q;
  if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
  return p;
}



inline Vec3 hslToRgb(double h, double s, double l){
  if(s == 0){
	return Vec3(l,l,l);
  } else{
	double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	double p = 2 * l - q;
	return Vec3(hue2rgb(p, q, h + 1/3), hue2rgb(p, q, h), hue2rgb(p, q, h - 1/3));
  }
}
*/
