#pragma once

#include <vector>
#include <string_view>
#include "DeformableObject.hpp"


class World{

public:

  World(std::string_view filename);


  std::vector<DeformableObject> dos;

};
