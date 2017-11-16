
#include <iostream>
#include <fstream>
#include <numeric>
#include "DeformableObject.hpp"


int main(int argc, char** argv){

  if(argc < 2){
	std::cout << "usage: ./adaptiveDeformables <filename>" << std::endl;
	return 1;
  }

  DeformableObject d(argv[1]);

  std::cout << d.particles.size() << std::endl;

  std::vector<int> indices(d.particles.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto clusters = kMeans(d, indices, 10);
  
  return 0;
}
