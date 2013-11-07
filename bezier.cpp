#include "bezier.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <algorithm>

/** Class Declarations */
class Point;
class Viewpoint;

/** 3D Point class */
class Point {
public:
  Point(float xpos, float ypos, float zpos) {
    x = xpos;
    y = ypos;
    z = zpos;
  }
  float x, y, z;
};

class Viewport {
public:
  int w, h; // width and height
};

// Global variables
Viewport viewport;


// Draws a curve
void Bezier::draw() {

}

/** Glut display method, loads identity matrix and draws */
void myDisplay() {
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
  // Indicate we are specifying camera transformations
  glMatrixMode(GL_MODELVIEW);
  // Zero the first transformation
  glLoadIdentity();
  

  glFlush();
  // We earlier set double buffers
  glutSwapBuffers();
}

/** Simple scene initialization */
void initScene(){
  // WHAT is htis?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!=--
  // Nothing to do here for this simple example.
}

/** Reshapes viewport if dragged and reshaped */
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);
}


void subdividePatch(std::vector<Point> patch, float step) {
  //finding the number of divisions:
  
  int numDiv = ((1 + epsilon) / step);
  
}

int main(int argc, char *argv[]) {
  /** File parsing: */

  int numPatches;
  std::vector< std::vector<Point> > patches;
  float subStep;
  std::string tesType;

  // Parsing command line for: input file name, the subdivision paramter, and
  // the flag which determines if subdivision should be adaptive or uniform:
  std::string file = std::string(argv[1]);
  subStep = atof(argv[2]);
  tesType = std::string(argv[3]);

  std::ifstream inpfile(file.c_str()); 
  if(!inpfile.is_open()) { 
    std::cout << "Unable to open file" << std::endl; 
  } else { 
    std::string line;
    std::vector<Point> patch;
    while(inpfile.good()) {

      std::vector<std::string> splitline; 
      std::string buf;

      std::getline(inpfile,line); 
      std::stringstream ss(line);

      while (ss >> buf) { 
	splitline.push_back(buf); 
      }

      // Ignore blank lines 
      if(splitline.size() == 0) { 
	continue; 
      } 
      if (splitline.size() == 1) { 
	numPatches = atoi(splitline[0].c_str()); 
	continue;//not sure if I need this 
      } else {
	Point p1(atof(splitline[0].c_str()), 
		 atof(splitline[1].c_str()), 
		 atof(splitline[2].c_str())); 
	Point p2(atof(splitline[3].c_str()), 
		 atof(splitline[4].c_str()), 
		 atof(splitline[5].c_str())); 
	Point p3(atof(splitline[6].c_str()), 
		 atof(splitline[7].c_str()), 
		 atof(splitline[8].c_str())); 
	Point p4(atof(splitline[9].c_str()), 
		 atof(splitline[10].c_str()), 
		 atof(splitline[11].c_str()));

	patch.push_back(p1); 
	patch.push_back(p2); 
	patch.push_back(p3); 
	patch.push_back(p4);

	// If patch size is 16, then make patch is done and look for next patch
	if (patch.size() == 16) {
	  patches.push_back(patch);
	  patch.clear();
	}
      }
    }
    inpfile.close();
  }

  /* Glut stuff */
  // Initialize glut
  glutInit(&argc, argv);

  // Double buffered window with RGB channels
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize the viewport size
  viewport.w = 400;
  viewport.h = 400;

  // Set up the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Bezier surfaces");

  // Quick setup of scene
  initScene();

  // Repeat callback handler
  glutDisplayFunc(myDisplay);
  // Resize window callback handler
  glutReshapeFunc(myReshape);
  
  // Glut main infinite loop
  glutMainLoop();
}

