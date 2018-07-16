#include <fstream>
#include <iostream>
#include <sstream>


#include <array>
#include <vector>
#include <string>


int main(int argc, char**argv){

  if(argc != 3){
	std::cout << "usage: obj2pts <input.obj> <output.pts>" << std::endl;
	return 1;
  }

  std::string infile(argv[1]);
  std::string outfile(argv[2]);

  std::ifstream ins(infile);
  std::ofstream outs(outfile, std::ios::binary);


  std::vector<std::array<double, 3> > points;
  std::string line;
  while(std::getline(ins, line)){
	if(line[0] == '#'){
	  continue;
	}

	std::istringstream lstream(line);
	char ch;
	std::array<double,3> vec;

	lstream >> ch >> vec[0] >> vec[1] >> vec[2];
	assert(ch == 'v');
	points.push_back(vec);
  }
  
  size_t np = points.size();
  outs.write(reinterpret_cast<const char*>(&np), sizeof(np));

  outs.write(reinterpret_cast<const char*>(points.data()), 3*sizeof(double)*np);

  return 0;
  
}
