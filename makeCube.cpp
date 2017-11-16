#include <fstream>

int main(int argc, char** argv){

  std::ofstream outs(argv[1], std::ios::binary);
  size_t N = 1000;
  outs.write(reinterpret_cast<const char*>(&N), sizeof(N));

  for(double x = 0; x < .99; x += .1){
	  for(double y = 0; y < .99; y += .1){
		  for(double z = 0; z < .99; z += .1){
			outs.write(reinterpret_cast<const char*>(&x), sizeof(x));
			outs.write(reinterpret_cast<const char*>(&y), sizeof(y));
			outs.write(reinterpret_cast<const char*>(&z), sizeof(z));
		  }
	  }
  }
  
  outs.close();
  return 0;
}
