cmake_minimum_required(VERSION 3.9)

project(adaptiveDeformables)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(adaptiveDeformables main.cpp DeformableObject.cpp World.cpp)



find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package (jsoncpp REQUIRED NO_MODULE)


target_link_libraries(adaptiveDeformables Eigen3::Eigen jsoncpp_lib)

