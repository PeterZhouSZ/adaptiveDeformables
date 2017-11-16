#include "World.hpp"
#include <fstream>


World::World(std::string_view filename){

  std::ifstream ins(filename.data(), std::ios::binary);
  
  Json::Value root;
  ins >> root;

  for(auto &dov : root["deformableObjects"]){
	dos.emplace_back(dov);
  }
  
}
