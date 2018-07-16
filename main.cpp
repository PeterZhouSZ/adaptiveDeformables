
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
  World world(argv[1]);
  DeformableObject d = world.dos.front();

  std::cout << d.particles.size() << std::endl;

  std::vector<int> indices(d.particles.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto clusters = kMeans(d, indices, 10);

  //world.duration = 0.01;

  double frameRate = 30;
  double timePerFrame = 1/frameRate;

  double nextFrame = 0;
  
  while(world.elapsedTime <= world.duration){
	if(world.elapsedTime >= nextFrame){
	  world.dumpMitsuba();
	  --world.frameNumber;
	  world.dump();
	  nextFrame += timePerFrame;
	}
	world.stepNoOvershoot();
	//world.step();
	
	std::cout << "time: " << world.elapsedTime << std::endl;
  }
  
  std::cout << "time spent simulation: " << world.simTime << std::endl;
  return 0;
}
