#include "DeformableObject.hpp"
#include <fstream>
#include <random>
#include <iostream>


DeformableObject::DeformableObject(const std::string& filename){


  std::ifstream ins(filename, std::ios::binary);

  size_t numParticles;
  ins.read(reinterpret_cast<char*>(&numParticles), sizeof(numParticles));

  particles.resize(numParticles);
  for(size_t i = 0; i < numParticles; ++i){
	ins.read(reinterpret_cast<char*>(&(particles[i].position.x())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.y())), sizeof(double));
	ins.read(reinterpret_cast<char*>(&(particles[i].position.z())), sizeof(double));
	std::cout << particles[i].position << '\n' << std::endl;
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
 
