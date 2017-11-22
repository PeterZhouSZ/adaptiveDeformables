#include "World.hpp"
#include <fstream>
#include <exception>

World::World(std::string_view filename){

  std::ifstream ins(filename.data(), std::ios::binary);
  
  Json::Value root;
  ins >> root;

  if(!root.isMember("dt")){
	throw std::runtime_error("input file must specify dt");
  }
  dt = root["dt"].asDouble();

  elapsedTime = 0;
  duration = root.get("duration", 10).asDouble();
  frameNumber = 0;
  
  for(auto &dov : root["deformableObjects"]){
	dos.emplace_back(dov);
  }
  
}


void World::step(){
  elapsedTime += dt;
  
  for(auto& d : dos){
	d.applyGravity(dt);
	d.applyElasticForces(dt);
	d.updatePositions(dt);
	d.bounceOffGround();
  }
  
}


void World::dump() {

  for(int i = 0; i < dos.size(); ++i){
	std::string fname = "output/frame." + std::to_string(frameNumber) + "." + std::to_string(i) + ".pts";
	dos[i].dump(fname);
  }
  ++frameNumber;
}
