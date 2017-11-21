#include "DeformableObject.hpp"
#include <fstream>
#include <random>
#include <iostream>
#include <json/json.h>

#include "AccelerationGrid.hpp"

DeformableObject::DeformableObject(const Json::Value& jv){


  std::string filename = jv["particleFile"].asString();
  std::ifstream ins(filename);



  if(!jv.isMember("lambda")){
	throw std::runtime_error("you didn't specify lambda for a deformable object");
  }
  lambda = jv["lambda"].asDouble();

  
  if(!jv.isMember("mu")){
	throw std::runtime_error("you didn't specify mu for a deformable object");
  }
  mu = jv["mu"].asDouble();

  
  if(!jv.isMember("density")){
	throw std::runtime_error("you didn't specify density for a deformable object");
  }
  density = jv["density"].asDouble();

  
  size_t numParticles;
  ins.read(reinterpret_cast<char*>(&numParticles), sizeof(numParticles));

  particles.resize(numParticles);
  for(size_t i = 0; i < numParticles; ++i){
	ins.read(reinterpret_cast<char*>(&(particles[i].position.x())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.y())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.z())), sizeof(double));

	particles[i].velocity = Vec3::Zero();
  }

  computeNeighbors();
  computeBasisAndVolume();
  
}


std::vector<int> kMeans(const DeformableObject& d,  const std::vector<int>& indices, int n){


  std::random_device rd;
  std::mt19937 gen(rd());

  std::vector<int> centers = indices;
  std::shuffle(centers.begin(), centers.end(), gen);
  centers.resize(n);

  std::sort(centers.begin(), centers.end());

  for(auto c : centers){
	std::cout << c << std::endl;
  }
  //number of points, and sum of those points positions
  std::vector<std::pair<int, Vec3> > centerInfo;
  

  for(int iter = 0; iter < 5; ++iter){
	auto oldCenters = centers;
	centerInfo.assign(centers.size(), std::pair<int, Vec3>(0, Vec3::Zero()));
	
	//assign each particle to nearest center
	for(const auto& p : d.particles){

	  int nc = std::distance(centers.begin(),
		  std::min_element(centers.begin(), centers.end(),
			  [&d, &p](int a, int b){
				return (d.particles[a].position - p.position).squaredNorm() <
				(d.particles[b].position - p.position).squaredNorm();
			  }));

	  ++centerInfo[nc].first;
	  centerInfo[nc].second += p.position;
	
	}


	//find particle nearest this sum

	for(auto i = 0; i < centers.size(); ++i){

	  Vec3 com = centerInfo[i].second/centerInfo[i].first;
	  std::cout << "com: " << com << std::endl;
	  centers[i] = *std::min_element(indices.begin(), indices.end(),
		  [&d, &com](int a, int b){
			return (com - d.particles[a].position).squaredNorm() <
			(com - d.particles[b].position).squaredNorm();
		  });
	  
	}
	std::cout << "iter: "<< iter << std::endl;
	for(auto c : centers){
	  std::cout << c << std::endl;
	}
	if(centers == oldCenters){
	  std::cout << "converged in " << iter << " iters" << std::endl;
	  break;
	}
  }

  return centers;
}
 

void DeformableObject::computeNeighbors(){

  struct GetPos{
	Vec3 operator()(const Particle& p) const { return p.position; }
  };

  int nb = std::sqrt(std::sqrt(particles.size())); //use 4th root of n buckets?
  AccelerationGrid<Particle, GetPos> ag(nb);
  ag.updateGrid(particles);
  
  auto delta = ag.getDelta();
  double radius = std::min({delta.x(), delta.y(), delta.z()});
  
  for(int i = 0; i < particles.size(); ++i){
	auto& p = particles[i];
	
	auto neighbors = ag.getNearestNeighbors(particles, p.position, radius);
	while(neighbors.size() < (desiredNumNeighbors() + 1)){ //count p itself, which won't be a neighbor
	  std::cout << "doubling radius" << std::endl;
	  radius *= 2;
	  neighbors = ag.getNearestNeighbors(particles, p.position, radius);
	}

	neighbors.erase(std::find(neighbors.begin(), neighbors.end(), i), neighbors.end());
	std::sort(neighbors.begin(), neighbors.end(),
		[this, &p](int a, int b){
		  return (particles[a].position - p.position).squaredNorm() <
			(particles[b].position - p.position).squaredNorm();
		});

	neighbors.resize(desiredNumNeighbors());

	p.neighbors.resize(neighbors.size());
	//set the radius to be the distance to the furthest neighbor
	p.kernelRadius = (particles[neighbors.back()].position - p.position).norm();
	
	for(auto j = 0; j < neighbors.size(); ++j){
	  p.neighbors[j].index = neighbors[j];
	  p.neighbors[j].uij = particles[neighbors[j]].position - p.position;
	  p.neighbors[j].wij = poly6(p.neighbors[j].uij.squaredNorm(), p.kernelRadius);
	}
	
  }



  /*for(const auto& p : particles){
	std::cout << "particle " << std::endl;
	for(const auto& pr : p.neighbors){
	  std::cout << pr.first << pr.second << std::endl;
	}
	std::cout << std::endl;
	}*/
}

void DeformableObject::applyGravity(double dt){
  for(auto& p : particles){
	p.velocity -= dt*Vec3(0, 9.81, 0);
  }
}


void DeformableObject::applyElasticForces(double dt){

  forces.assign(particles.size(), Vec3::Zero());
  
  for(int i = 0; i < particles.size(); ++i){
	auto& p = particles[i];

	Mat3 ux = Mat3::Zero();
	for(const auto& n : p.neighbors){
	  ux += n.wij*(p.position - particles[n.index].position)*n.uij.transpose();
	}
	
	Mat3 F = ux*p.Ainv;

	Eigen::JacobiSVD<Mat3> svd(F, Eigen::ComputeFullU | Eigen::ComputeFullV);
	Vec3 FHat = svd.singularValues();
	Vec3 diagStress = lambda*(FHat(0) + FHat(1) + FHat(2) - 3)*Vec3(1,1,1) +
	  2*mu*(FHat - Vec3(1,1,1));

	Mat3 rotStress = svd.matrixU()*diagStress.asDiagonal()*svd.matrixV().transpose();

	Mat3 forceCommon = p.volume*rotStress*p.Ainv;
	
	for(const auto& n : p.neighbors){
	  Vec3 fi = forceCommon*(n.wij*n.uij);
	  forces[i] += fi;
	  forces[n.index] -= fi;
	}
	
  }

  //now modify velocities
  for(int i = 0; i < particles.size(); ++i){
	particles[i].velocity += forces[i]/(particles[i].volume*density);
  }
  
}


void DeformableObject::updatePositions(double dt){
  for(auto& p : particles){
	p.position += dt*p.velocity;
  }
  
}


void DeformableObject::computeBasisAndVolume(){
  
  for(auto& p : particles){
	Mat3 A = Mat3::Zero();
	double wSum = 0;
	for(const auto& n : p.neighbors){
	  A += n.wij* n.uij* n.uij.transpose();
	  wSum += n.wij;
	}
	p.Ainv = A.inverse();
	p.volume = std::sqrt(A.determinant()/std::pow(wSum, 3));
  }
  
}
