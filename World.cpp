#include "World.hpp"
#include <fstream>
#include <exception>
#include <cassert>

World::World(std::string_view filename){

  std::ifstream ins(filename.data(), std::ios::binary);
  
  Json::Value root;
  ins >> root;

  dt = getOrThrow<double>(root, "dt");

  elapsedTime = 0;
  duration = root.get("duration", 10).asDouble();
  frameNumber = 0;
  
  for(auto &dov : root["deformableObjects"]){
	dos.emplace_back(dov);
	//	dos.back().writeHierarchy(std::string{"output/hierarchy."} +
	//		std::to_string(dos.size() -1) +
	//		std::string(".hier"));
  }

  simTime = 0;

  mitsubaInfo = root["mitsubaInfo"].asString();
  
}


void World::step(){

  Stopwatch sw;
  sw.start();
  
  elapsedTime += dt;
  for(auto& d : dos){
	d.applyGravity(dt);

	d.applyElasticForces(dt);
	//d.applyElasticForcesAdaptive(dt);

	d.damp(dt);

	d.updatePositions(dt);
	d.bounceOffGround();

	double eng = 0;
	for(const auto & p : d.particles){
	  auto m = d.density*p.volume;
	  eng += m*p.position.y() + m*p.velocity.squaredNorm();
	}
	std::cout << "eng: " << eng << std::endl;
  }
  simTime += sw.stop();
}


void World::stepNoOvershoot(){
  Stopwatch sw;
  sw.start();
  
  elapsedTime += dt;
  for(auto& d : dos){
	d.applyGravity(dt);

	d.applyElasticForcesNoOvershoot(dt);
	d.damp(dt);

	d.updatePositions(dt);
	d.bounceOffGround();
  }
  simTime += sw.stop();

}


void World::dump() {

  for(int i = 0; i < dos.size(); ++i){
	std::string fname = "output/frame." + std::to_string(frameNumber) + "." + std::to_string(i) + ".pts";
	dos[i].dump(fname);
	//dos[i].dumpWithColor(fname);
  }
  ++frameNumber;
}


void World::dumpMitsuba(){

  char fNumber[8];
  sprintf(fNumber, "%04d", frameNumber);
  std::ofstream outs("output/mitsubaFrame_" + std::string(fNumber) + ".xml");
  outs << R"noise(<?xml version="1.0" encoding="utf-8"?>
<scene version="0.5.0">)noise" << std::endl;

  outs << mitsubaInfo << std::endl;
  
  
  const std::string sphereStart = "<shape type=\"sphere\">\n<point name=\"center\" ";
  const std::string sphereEnd = "</shape>\n";
  
  for(const auto& dobj : dos){
	for(int i = 0; i < dobj.particles.size(); ++i){

	  outs << sphereStart;
	  outs << "x=\"" << dobj.particles[i].position.x() << "\" " 
		   << "y=\"" << dobj.particles[i].position.y() << "\" " 
		   << "z=\"" << dobj.particles[i].position.z() << "\" />";
	  outs << "<float name=\"radius\" value=\"" << dobj.renderInfos[i].size << "\" />\n";
	  outs << "<bsdf type=\"diffuse\"><rgb name=\"reflectance\" value=\""
		   << dobj.renderInfos[i].color.x() << ", "
		   << dobj.renderInfos[i].color.y() << ", "
		   << dobj.renderInfos[i].color.z() 
		   << "\" /></bsdf>\n";
	  
	  outs << sphereEnd << std::endl;
	  
	}
	
  }
  outs << "</scene>" << std::endl;
  ++frameNumber;

}
