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
  
  for(auto &dov : root["deformableObjects"]){
	dos.emplace_back(dov);
  }
  
}



void World::step(){


  for(auto& d : dos){
	d.applyGravity(dt);

	d.applyElasticForces(dt);

	d.updatePositions(dt);
  }

  
  
  
}
