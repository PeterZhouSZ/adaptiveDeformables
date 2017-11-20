#include "DeformableObject.hpp"
#include <fstream>
#include <random>
#include <iostream>
#include <json/json.h>

#include "AccelerationGrid.hpp"

DeformableObject::DeformableObject(const Json::Value& jv){


  std::string filename = jv["particleFile"].asString();
  std::ifstream ins(filename);
  
  size_t numParticles;
  ins.read(reinterpret_cast<char*>(&numParticles), sizeof(numParticles));

  particles.resize(numParticles);
  for(size_t i = 0; i < numParticles; ++i){
	ins.read(reinterpret_cast<char*>(&(particles[i].position.x())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.y())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.z())), sizeof(double));

	particles[i].velocity = Vec3::Zero();
  }
  
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
	for(auto j = 0; j < neighbors.size(); ++j){
	  p.neighbors[j].first = neighbors[j];
	  p.neighbors[j].second = particles[neighbors[j]].position - p.position;
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
  
}


void DeformableObject::updatePositions(double dt){
  for(auto& p : particles){
	p.position += dt*p.velocity;
  }
  
}
