
#include <iostream>
#include <fstream>
#include <numeric>
#include "World.hpp"
#include "DeformableObject.hpp"


int main(int argc, char** argv){

  if(argc < 2){
	std::cout << "usage: ./adaptiveDeformables <filename>" << std::endl;
	return 1;
  }

  World w(argv[1]);
  DeformableObject d = w.dos.front();

  std::cout << d.particles.size() << std::endl;

  std::vector<int> indices(d.particles.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto clusters = kMeans(d, indices, 10);


  d.computeNeighbors();
  
  return 0;
}
