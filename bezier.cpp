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

//global variables
Viewport viewport;


//draws a curve
void Bezier::draw() {

}


//function that draws things and loads matrix identity
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

// Simple init function
void initScene(){
  // WHAT is htis?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!=--
  // Nothing to do here for this simple example.
}

/* Reshapes viewport if dragged and reshaped */
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);
}


void subdividePatch(std::vector<Point> patch, float step) {
  //setting the epsilon value:
  float epsilon = 0.000001;

  //the parametric values u, v
  float u;
  float v;

  //finding the number of divisions:
  int numDiv = ((1 + epsilon) / step);

  //for each parametric value of u:
  for (float iu = 0; iu < numDiv; iu++ ) {
    u = iu * step;

    //for each parametric value of v:
    for (float iv = 0; iv < numDiv; iv++) {
      v = iv * step;

      //evaluating the surface:
      bezPatchInterp(patch, u, v);

      //save the surfacepoint and normal somehwere??!!!
      //saveSurfacePointAndNormal(p, n);
    }
  }
}

//given the control patch and (u, v) values, find the surface
//point and normal
void bezPatchInterp(std::vector<Point> patch, float u, float v) {
  //the v and u curves:
  std::vector<Point> vcurve;
  std::vector<Point> ucurve;

  //the control points for the curves in v and u:
  std::vector<Point> vCurveControlPoints;
  std::vector<Point> uCurveControlPoints;

  //build the control points for a Bezier curve in v:
  //where patch[index] = Point(x, y, z)
  vCurveControlPoints.push_back(patch[0]);
  vCurveControlPoints.push_back(patch[1]);
  vCurveControlPoints.push_back(patch[2]);
  vCurveControlPoints.push_back(patch[3]);
  vcurve.push_back(bezCurveInterp(vCurveControlPoints, u);
  vCurveControlPoints.clear();

  vCurveControlPoints.push_back(patch[4]);
  vCurveControlPoints.push_back(patch[5]);
  vCurveControlPoints.push_back(patch[6]);
  vCurveControlPoints.push_back(patch[7]);
  vcurve.push_back(bezCurveInterp(vCurveControlPoints, u);
  vCurveControlPoints.clear();

  vCurveControlPoints.push_back(patch[8]);
  vCurveControlPoints.push_back(patch[9]);
  vCurveControlPoints.push_back(patch[10]);
  vCurveControlPoints.push_back(patch[11]);
  vcurve.push_back(bezCurveInterp(vCurveControlPoints, u);
  vCurveControlPoints.clear();

  vCurveControlPoints.push_back(patch[12]);
  vCurveControlPoints.push_back(patch[13]);
  vCurveControlPoints.push_back(patch[14]);
  vCurveControlPoints.push_back(patch[15]);
  vcurve.push_back(bezCurveInterp(vCurveControlPoints, u);
  vCurveControlPoints.clear();

  //build the control points for a Bezier curve in u:
  //(same process as above):
  uCurveControlPoints.push_back(patch[0]);
  uCurveControlPoints.push_back(patch[4]);
  uCurveControlPoints.push_back(patch[8]);
  uCurveControlPoints.push_back(patch[12]);
  std::vector<Point> tuple = bezCurveInterp(uCurveControlPoints, v)
  ucurve.push_back(tuple[0]);
  uCurveControlPoints.clear();

  uCurveControlPoints.push_back(patch[1]);
  uCurveControlPoints.push_back(patch[5]);
  uCurveControlPoints.push_back(patch[9]);
  uCurveControlPoints.push_back(patch[13]);
  ucurve.push_back(bezCurveInterp(uCurveControlPoints, v);
  uCurveControlPoints.clear();

  uCurveControlPoints.push_back(patch[2]);
  uCurveControlPoints.push_back(patch[6]);
  uCurveControlPoints.push_back(patch[10]);
  uCurveControlPoints.push_back(patch[14]);
  ucurve.push_back(bezCurveInterp(uCurveControlPoints, v);
  uCurveControlPoints.clear();

  uCurveControlPoints.push_back(patch[3]);
  uCurveControlPoints.push_back(patch[7]);
  uCurveControlPoints.push_back(patch[11]);
  uCurveControlPoints.push_back(patch[15]);
  ucurve.push_back(bezCurveInterp(uCurveControlPoints, v);
  uCurveControlPoints.clear();

  //evaluate surface and derivative for u and v:
  // p, dPdv = bezCurveInterp(vcurve, v);
  // p, dPdv = bezCurveInterp(ucurve, u);


  Point surfacePointP = bezCurveInterp(vcurve, v);
  Point surfacePointU = bezCurveInterp(ucurve, u);


  //take the crossproduct of partials to find normal
  //vector<float> n = cross(dPdu, dPdv);
  //n = n / n.size();

  //return p, n;

}

//given the control points of a bezier curve and a parametric
//value, return a vector of size 2 containing: the curve point
//and derivative point:
std::vector<Point> bezCurveInterp(std::vector<Point> curve, float u) {
  //first, split each of the three segments to form
  // two new ones AB and BC:
  //Lizzie question: can I multiply points like that? 
  Point A(curve[0].x * (1.0 - u) + curve[1].x * u,
          curve[0].y * (1.0 - u) + curve[1].y * u,
          curve[0].z * (1.0 - u) + curve[1].z * u);

  Point B(curve[1].x * (1.0 - u) + curve[2].x * u,
          curve[1].y * (1.0 - u) + curve[2].y * u,
          curve[1].z * (1.0 - u) + curve[2].z * u);

  Point C(curve[2].x * (1.0 - u) + curve[3].x * u,
          curve[2].y * (1.0 - u) + curve[3].y * u,
          curve[2].z * (1.0 - u) + curve[3].z * u);

  //Now, split AB and BC to form a new segment DE:
  Point D(A.x * (1.0 - u) + B.x * u,
          A.y * (1.0 - u) + B.y * u,
          A.z * (1.0 - u) + B.z * u);

  Point E(B.x * (1.0 - u) + C.x * u,
          B.y * (1.0 - u) + C.y * u,
          B.z * (1.0 - u) + C.z * u);

  //Finally, picking the right point on DE:
  Point finalP(D.x * (1.0 - u) + E.x * u,
                D.y * (1.0 - u) + E.y * u,
                D.z * (1.0 - u) + E.z * u);

  //computing the derivative:
  //finding Point E - Point D:
  Point EMinusD(E.x - D.x, E.y - D.y, E.z - D.z);
  Point derivP(3 * EMinusD.x, 3 * EMinusD.y, 3 * EMinusD.z);

  //finalTuple will contain: (finalP, derivativeP)
  std::vector<Point> finalTuple;
  finalTuple[0] = finalP;
  finalTuple[1] = derivP;


  return finalTuple;
}


  


int main(int argc, char *argv[]) {

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  // Quick setup of scene
  initScene();

  // Function to run something when the time has c0me
  glutDisplayFunc(myDisplay);
  // Function to run when resize window
  glutReshapeFunc(myReshape);
  
  // Infinite loop that will keep drawing and resizing
  glutMainLoop();
  // and w4t3v3r else



  

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
  





}

