cmake_minimum_required(VERSION 3.9)

project(adaptiveDeformables)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(simulator main.cpp DeformableObject.cpp World.cpp)



find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package (jsoncpp REQUIRED NO_MODULE)
find_package (GLUT REQUIRED)
find_package (OpenGL REQUIRED)

target_link_libraries(simulator Eigen3::Eigen jsoncpp_lib)

add_executable(openglViewer openglViewer.cpp)
target_include_directories(openglViewer PRIVATE ${GLUT_INCLUDE_DIR})
target_link_libraries(openglViewer Eigen3::Eigen ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} jsoncpp_lib)

