#pragma once
#include <chrono>

class Stopwatch{

public:

  void start(){
	startTime = std::chrono::high_resolution_clock::now();
  }

  double stop(){
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
  }
private:
  
  std::chrono::high_resolution_clock::time_point startTime;
};
