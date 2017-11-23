#include<iostream>
#include<vector>
#include<fstream>
#include <cstdlib>
#include "Util.hpp"


#include <glut.h>


double randDouble(){
  return (double)(rand())/((double)(RAND_MAX));
}

std::string makeFilename(const std::string &format, unsigned frame);

//read globalPInfo and frameNumber and worldFormat from global vars
void readFrame();

void displayFrame();
void keyInput(unsigned char key, int x, int y);
void specialInput(int key, int x, int y);
void reshape(int width, int height);
void mouseClicks(int button, int state, int x, int y);
void mouseMove(int x, int y);

std::vector<Vec3> colors;
std::vector<std::vector<Vec3>> positions;


Vec3 center;//for gluLookat
Vec3 eye;
Vec3 upVector;
unsigned currentFrame = 0;
std::string worldPrefix;

int windWidth, windHeight;
double zoomFactor = 1.0;

float lightPos1[4] = {0.0f, 20.0f, 90.0f, 0.0f};
float lightPos2[4] = {3.0f, 20.0f, -30.0f, 0.0f};
float lightColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightAmbient[4] = {.1f, .1f, .1f, 1.0f};

int main(int argc, char** argv){
  if(argc < 2){
    std::cout << "Usage: ./openglViewer worldPrefix \n"
              << "(eg output/frame )\n";
    exit(1);
  }
  worldPrefix = std::string(argv[1]);

  readFrame();
  


  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
  glutInitWindowSize(800,800);
  windWidth = windHeight = 800;

  glutCreateWindow("Particle Viewer");
  glutDisplayFunc(displayFrame);
  glutKeyboardFunc(keyInput);
  glutSpecialFunc(specialInput);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouseClicks);
  glutMotionFunc(mouseMove);

  glClearColor(0,0,0,0);
  glEnable(GL_DEPTH_TEST);

  center[0] = center[1] = center[2] = 0;
  center[1] = -2;
  eye[0] = 0;
  eye[1] = 5;
  eye[2] = 10;

  upVector[0] = upVector[2] = 0;
  upVector[1] = 1;  

  glutMainLoop();
}


void readFrame(){
  std::cout << "loading frame: " << currentFrame << std::endl;
  for(int i = 0; ; ++i){

	std::string filename = worldPrefix + "." +
	  std::to_string(currentFrame) + "." + std::to_string(i) + ".pts";
	std::cout << "trying to read " << filename << std::endl;
    std::ifstream inWorld(filename, std::ios::binary);
	if(!inWorld){
	  if(i == 0){
		std::cout << "mo more frames" << std::endl;
		--currentFrame;
	  } 
	  break;
	}

	size_t numPoints;
	size_t start = 0;
	inWorld.read(reinterpret_cast<char*>(&numPoints), sizeof(numPoints));
	std::cout << "points in file: " << numPoints << std::endl;
	if(i == 0){
	  positions.push_back(std::vector<Vec3>(numPoints));
	} else {
	  start = positions.back().size();
	  positions.back().resize(positions.size() + numPoints);
	}

	inWorld.read(reinterpret_cast<char*>(positions.back().data() + start), numPoints*3*sizeof(double));
  }
  std::cout << "num points: " << positions.back().size() << std::endl;
  if(colors.empty()){
	assert(currentFrame == 0);
	colors.resize(positions.front().size());
	for(auto& c : colors){
	  c.x() = randDouble();
	  c.y() = randDouble();
	  c.z() = randDouble();
	}
  }
}


void displayFrame(){

  Vec3 COM = Vec3::Zero();
  assert(positions.size() > currentFrame);
  for(const auto& v : positions[currentFrame]){
	COM += v;
  }
  
  COM /= positions[currentFrame].size();
  center = COM;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glShadeModel(GL_SMOOTH);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);  
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

  glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);  
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
  glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);


  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2],
	    upVector[0], upVector[1], upVector[2]);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40*zoomFactor, (double)(windWidth)/windHeight, .5, 100);


  glColor3d(.6,.6,.6);
  glBegin(GL_QUADS);
  glVertex3d(-10,0,10);
  glVertex3d(-10,0, -10);
  glVertex3d(10, 0, -10);
  glVertex3d(10, 0, 10);
  glEnd();
  
  glMatrixMode(GL_MODELVIEW);
  for (unsigned i = 0; i < colors.size(); ++i) {
	glColor3dv(colors[i].data());
	
	glPushMatrix();
	if(!positions[currentFrame][i].allFinite()){
	  std::cout << "nan particle" << std::endl;
	}
	glTranslated(positions[currentFrame][i].x(),
		positions[currentFrame][i].y(),
		positions[currentFrame][i].z());
	
	glutSolidSphere(.01, 10,10);
	glPopMatrix();
	
  }
  
  
  
  
  
  glFlush();
  glutSwapBuffers();
}

void keyInput(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC
        exit(0);
		
    case '0':
	  currentFrame = 1;
	  specialInput(GLUT_KEY_LEFT, 0, 0);
	  break;
    }
}

void specialInput(int key, int x, int y){
  if(key == GLUT_KEY_RIGHT){
	int times = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 10 : 1;
    std::cout << "right pressed" << std::endl;
	for(int i = 0; i < times; ++i){
	  currentFrame++;
	  std::cout << "currentFrame: " <<currentFrame << std::endl;
	  if(positions.size() <= currentFrame){
		readFrame();
	  }
	}
    glutPostRedisplay();
  } else if (key == GLUT_KEY_LEFT){
    std::cout << "left pressed" << std::endl;
    if(currentFrame != 0)
      --currentFrame;
    std::cout << "currentFrame: " <<currentFrame << std::endl;
    glutPostRedisplay();
  } else if(key == GLUT_KEY_UP){
    zoomFactor /= 1.1;
    glutPostRedisplay();
  } else if(key == GLUT_KEY_DOWN){
    zoomFactor *= 1.1;
    glutPostRedisplay();
  }
}

void reshape(int width, int height){
  glViewport(0,0,width, height);
  windWidth = width;
  windHeight = height;
  glutPostRedisplay();
}

int lastMouseX, lastMouseY;
void mouseClicks(int button, int state, int x, int y){
  lastMouseX = x; lastMouseY = y;
}

void mouseMove(int x, int y){
  Vec3 eyeToCenter = center - eye;
  double dist = eyeToCenter.norm();
  Vec3 xDirection = eyeToCenter.cross(upVector);
  xDirection.normalize();
  Vec3 yDirection = xDirection.cross(eyeToCenter);
  yDirection.normalize();

  double dx = x - lastMouseX;
  double dy = y - lastMouseY;
  lastMouseX = x;
  lastMouseY = y;

  eyeToCenter -= (-4*dx)/(.5*windWidth) * xDirection + (4*dy)/(.5*windHeight) * yDirection;


  eyeToCenter.normalize();
  eyeToCenter = eyeToCenter*dist;


  eye = center - eyeToCenter;
  glutPostRedisplay();
  
}

