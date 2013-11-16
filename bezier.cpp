#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <stdexcept>
#include <time.h>
#include <math.h>

/** Class Declarations */
class Point;
class Viewpoint;

/** Method Declarations */
void unifSubdividePatch(std::vector<Point> patch, float step);
void adapSubdividePatch(std::vector<Point> patch, std::vector<Point> pt1, 
			std::vector<Point> pt2, std::vector<Point> pt3, float u1, 
			float v1, float u2, float v2, float u3, float v3, 
			float tolerance);
std::vector<Point> bezSurfacifier(std::vector<Point> bsCPoints, float u, float v);
std::vector<Point> bezCurvifier(Point p0, Point p1, Point p2, Point p3, float t);
void unifTesselator();
void adapTesselator();
void cameraSetUp();
void lightSetUp();

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

/** Viewport class  */
class Viewport {
public:
  int w, h; // width and height
};


/** Global variables */
Viewport viewport;
std::vector<int> numPatches;
std::vector< std::vector<Point> > patches;
//bigPatches is a giant vector that holds the patches for each
//object that is drawn in the scene
std::vector< std::vector< std::vector<Point> > > bigPatches;
float subStep;
int sqrtNumQuads;
std::string tesType;

float zoom = 0.0;
float rotateX = 0.0;
float rotateY = 0.0;
float translateX = 0.0;
float translateY = 0.0;

bool isWireframe;
bool isFlat;
bool isS = 0;

/** Sets up the lights and light properties in the scene. */
void lightSetUp() {

  // defining light attributes:
  GLfloat ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
  GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat position[] = {1.0f, 1.0f, 1.0f, 0.0f};
  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f };

  //creating and making light0 with the above attributes:
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);


  //enabling the lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}


/** Reshapes viewport if dragged and reshaped */
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;
  glViewport (0,0,viewport.w,viewport.h);
  // Projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-5.0, 5.0, -5.0, 5.0, 1, 30);
}

/** Simple scene initialization */
void initScene(){

  // Set sqrtNumQuads
  sqrtNumQuads = 0;
  for (float i = 0; i < 1.0f; i += subStep) {
    sqrtNumQuads++;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
  myReshape(viewport.w,viewport.h);

  //Clear the depth buffer
  glClearDepth(1);

  //Enables depth testing (for lights)
  glEnable(GL_DEPTH_TEST);
}

/** Glut display method, loads identity matrix and draws */
void myDisplay() {


  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Indicate we are specifying camera transformations
  glMatrixMode(GL_MODELVIEW);
  // Zero the first transformation
  glLoadIdentity();
  // Camera
  gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);


  //glScalef(zoom, zoom, zoom);////

  // Light scene if isS
  if (isS)
    lightSetUp();

  glPushMatrix();
  glTranslatef(translateX, translateY, 0.0f);
  glRotatef(rotateX, 1.0, 0.0, 0.0 );
  glRotatef(rotateY, 0.0, 0.0, 1.0 );
  
  if (!tesType.compare("-u")) {
    unifTesselator();
  } else if (!tesType.compare("-a")) {
    adapTesselator();
  }

  glPopMatrix();

  glFlush();
  // We earlier set double buffers
  glutSwapBuffers();
}

/** Draw uniform tesselation form */
void unifTesselator() {
  for (int p = 0; p < bigPatches.size(); p++) {
    std::vector< std::vector<Point> > patches = bigPatches[p];
    for (int i = 0; i < patches.size(); i++) {
      // Draws entire patch uniformly
      unifSubdividePatch(patches[i], subStep);
    }
    if (bigPatches.size() == 0) {
      for (int i = 0; i < patches.size(); i++) {
        // Draws entire patch uniformly
        unifSubdividePatch(patches[i], subStep);
      }
    } else {
      for (int p = 0; p < bigPatches.size(); p++) {
        std::vector< std::vector<Point> > patches = bigPatches[p];
        for (int i = 0; i < patches.size(); i++) {
          // Draws entire patch uniformly
          unifSubdividePatch(patches[i], subStep);
	}
      }

    }

  }

}

/** Draw adaptive tesselation form  */
void adapTesselator() {
  for (int p = 0; p < bigPatches.size(); p++) {
    std::vector< std::vector<Point> > patches = bigPatches[p];
    for (int i = 0; i < patches.size(); i++) {
      // Set up initial recursion case
      std::vector<Point> tl, bl, br, tr;
      tl = bezSurfacifier(patches[i], 0.0f, 0.0f);
      bl = bezSurfacifier(patches[i], 0.0f, 1.0f);
      br = bezSurfacifier(patches[i], 1.0f, 1.0f);
      tr = bezSurfacifier(patches[i], 1.0f, 0.0f);
      // Draws entire patch adaptively
      adapSubdividePatch(patches[i], tl, bl, tr, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
			 subStep);
      adapSubdividePatch(patches[i], tr, bl, br, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
			 subStep);
    }
    if (bigPatches.size() == 0) {
      for (int i = 0; i < patches.size(); i++) {
	// Set up initial recursion case
	std::vector<Point> tl, bl, br, tr;
	tl = bezSurfacifier(patches[i], 0.0f, 0.0f);
	bl = bezSurfacifier(patches[i], 0.0f, 1.0f);
	br = bezSurfacifier(patches[i], 1.0f, 1.0f);
	tr = bezSurfacifier(patches[i], 1.0f, 0.0f);
	// Draws entire patch adaptively
	adapSubdividePatch(patches[i], tl, bl, tr, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
			   subStep);
	adapSubdividePatch(patches[i], tr, bl, br, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
			   subStep);
      }

    } else {
      for (int p = 0; p < bigPatches.size(); p++) {
	std::vector< std::vector<Point> > patches = bigPatches[p];
	for (int i = 0; i < patches.size(); i++) {
	  // Set up initial recursion case
	  std::vector<Point> tl, bl, br, tr;
	  tl = bezSurfacifier(patches[i], 0.0f, 0.0f);
	  bl = bezSurfacifier(patches[i], 0.0f, 1.0f);
	  br = bezSurfacifier(patches[i], 1.0f, 1.0f);
	  tr = bezSurfacifier(patches[i], 1.0f, 0.0f);
	  // Draws entire patch adaptively
	  adapSubdividePatch(patches[i], tl, bl, tr, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
			     subStep);
	  adapSubdividePatch(patches[i], tr, bl, br, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
			     subStep);
	}

      }

    }
  }

}

/** Given a triangle and their respective u v coords, recursively resize triangles
 *  adaptively. */
void adapSubdividePatch(std::vector<Point> patch, std::vector<Point> pt1, 
			std::vector<Point> pt2, std::vector<Point> pt3, float u1, 
			float v1, float u2, float v2, float u3, float v3, 
			float tolerance) {
  // Midpoint primitive points
  Point pp12((pt1[0].x + pt2[0].x)/2, (pt1[0].y + pt2[0].y)/2, 
	     (pt1[0].z + pt2[0].z)/2);
  Point pp23((pt2[0].x + pt3[0].x)/2, (pt2[0].y + pt3[0].y)/2, 
	     (pt2[0].z + pt3[0].z)/2);
  Point pp31((pt3[0].x + pt1[0].x)/2, (pt3[0].y + pt1[0].y)/2, 
	     (pt3[0].z + pt1[0].z)/2);
  // Midpoint u and v
  float u12 = (u1 + u2)/2;
  float v12 = (v1 + v2)/2;
  float u23 = (u2 + u3)/2;
  float v23 = (v2 + v3)/2;
  float u31 = (u3 + u1)/2;
  float v31 = (v3 + v1)/2;
  // Midpoint bezier surface points (bsp)
  std::vector<Point> bsp12 = bezSurfacifier(patch, u12, v12);
  std::vector<Point> bsp23 = bezSurfacifier(patch, u23, v23);
  std::vector<Point> bsp31 = bezSurfacifier(patch, u31, v31);
  
  // Calculate distances
  float d12 = sqrt(pow(pp12.x - bsp12[0].x, 2) + 
		   pow(pp12.y - bsp12[0].y, 2) +
		   pow(pp12.z - bsp12[0].z, 2));
  float d23 = sqrt(pow(pp23.x - bsp23[0].x, 2) + 
		   pow(pp23.y - bsp23[0].y, 2) + 
		   pow(pp23.z - bsp23[0].z, 2));
  float d31 = sqrt(pow(pp31.x - bsp31[0].x, 2) + 
		   pow(pp31.y - bsp31[0].y, 2) + 
		   pow(pp31.z - bsp31[0].z, 2));
  // Pass tolerance test
  int isPass12 = (d12 <= tolerance);
  int isPass23 = (d23 <= tolerance);
  int isPass31 = (d31 <= tolerance);

  // Case handling for recursion
  if (isPass12 && isPass23 && isPass31) {
    glBegin(GL_TRIANGLES);
    // Top left
    glNormal3f(pt1[1].x, pt1[1].y, pt1[1].z);
    glVertex3f(pt1[0].x, pt1[0].y, pt1[0].z);
    // Bottom left
    glNormal3f(pt2[1].x, pt2[1].y, pt2[1].z);
    glVertex3f(pt2[0].x, pt2[0].y, pt2[0].z);
    // Top right
    glNormal3f(pt3[1].x, pt3[1].y, pt3[1].z);
    glVertex3f(pt3[0].x, pt3[0].y, pt3[0].z);
    glEnd();
  } else if (isPass12 && isPass23) {
    // isPass31 fails
    adapSubdividePatch(patch, bsp31, pt1, pt2, u31, v31, u1, v1, u2, v2, subStep);
    adapSubdividePatch(patch, bsp31, pt2, pt3, u31, v31, u2, v2, u3, v3, subStep);    
  } else if (isPass23 && isPass31) {
    // isPass12 fails
    adapSubdividePatch(patch, bsp12, pt2, pt3, u12, v12, u2, v2, u3, v3, subStep);
    adapSubdividePatch(patch, bsp12, pt3, pt1, u12, v12, u3, v3, u1, v1, subStep);    
  } else if (isPass12 && isPass31) {
    // isPass23 fails
    adapSubdividePatch(patch, bsp23, pt3, pt1, u23, v23, u3, v3, u1, v1, subStep);
    adapSubdividePatch(patch, bsp23, pt1, pt2, u23, v23, u1, v1, u2, v2, subStep);    
  } else if (isPass12) {
    // isPass23 and isPass31 fail
    adapSubdividePatch(patch, bsp23, pt1, pt2, u23, v23, u1, v1, u2, v2, subStep);
    adapSubdividePatch(patch, bsp23, bsp31, pt1, u23, v23, u31, v31, u1, v1, subStep);
    adapSubdividePatch(patch, bsp23, pt3, bsp31, u23, v23, u3, v3, u31, v31, subStep);
  } else if (isPass23) {
    // isPass12 and isPass31 fail
    adapSubdividePatch(patch, bsp31, pt2, pt3, u31, v31, u2, v2, u3, v3, subStep);
    adapSubdividePatch(patch, bsp31, bsp12, pt2, u31, v31, u12, v12, u2, v2, subStep);
    adapSubdividePatch(patch, bsp31, pt1, bsp12, u31, v31, u1, v1, u12, v12, subStep);
  } else if (isPass31) {
    // isPass12 and isPass23 fail
    adapSubdividePatch(patch, bsp12, pt3, pt1, u12, v12, u3, v3, u1, v1, subStep);
    adapSubdividePatch(patch, bsp12, bsp23, pt3, u12, v12, u23, v23, u3, v3, subStep);
    adapSubdividePatch(patch, bsp12, pt2, bsp23, u12, v12, u2, v2, u23, v23, subStep);
  } else {
    // All fail
    adapSubdividePatch(patch, pt1, bsp12, bsp31, u1, v1, u12, v12, u31, v31, subStep);
    adapSubdividePatch(patch, pt2, bsp23, bsp12, u2, v2, u23, v23, u12, v12, subStep);
    adapSubdividePatch(patch, pt3, bsp31, bsp23, u3, v3, u31, v31, u23, v23, subStep);
    adapSubdividePatch(patch, bsp12, bsp23, bsp31, u12, v12, u23, v23, u31, v31, 
		       subStep);
  }
}

/** Given a patch containing 16 control points, draw entire patch after uniform
 *  subdivision */
void unifSubdividePatch (std::vector<Point> patch, float step) {
  // the parametric values u, v
  float u, v;

  for (int i = 0; i < sqrtNumQuads; i++) {
    v = step * i;
    for (int j = 0; j < sqrtNumQuads; j++) {
      u = step * j;
      std::vector<Point> tl, bl, br, tr;
      // Top left
      tl = bezSurfacifier(patch, u, v);
      // Bottom left
      if (v + step >= 1.0f) { 
	bl = bezSurfacifier(patch, u, 1.0f);
      } else {
	bl = bezSurfacifier(patch, u, v + step);
      }
      // Bottom right
      if (v + step >= 1.0f && u + step >= 1.0f) { 
	br = bezSurfacifier(patch, 1.0f, 1.0f);
      } else if (v + step >= 1.0f) {
	br = bezSurfacifier(patch, u + step, 1.0f);
      } else if (u + step >= 1.0f) {
	br = bezSurfacifier(patch, 1.0f, v + step);
      } else {
	br = bezSurfacifier(patch, u + step, v + step);
      }
      // Top right
      if (u + step >= 1.0f) { 
	tr = bezSurfacifier(patch, 1.0f, v);
      } else {
	tr = bezSurfacifier(patch, u + step, v);
      }
      
      glBegin(GL_QUADS);
      glNormal3f(tl[1].x, tl[1].y, tl[1].z);
      glVertex3f(tl[0].x, tl[0].y, tl[0].z); //top left
      glNormal3f(bl[1].x, bl[1].y, bl[1].z);
      glVertex3f(bl[0].x, bl[0].y, bl[0].z); //bottom left
      glNormal3f(br[1].x, br[1].y, br[1].z);
      glVertex3f(br[0].x, br[0].y, br[0].z); //bottom right
      glNormal3f(tr[1].x, tr[1].y, tr[1].z);
      glVertex3f(tr[0].x, tr[0].y, tr[0].z); //top right
      glEnd();
    }
  }
  
}

/** Create SURFACE bezier point, given u and v of bezier surface control points 
 *  bsCPoints == bezier surface Control Points
 *  Returns size-2 vector, 2nd of which is the vertex normal */
std::vector<Point> bezSurfacifier(std::vector<Point> bsCPoints, float u, float v) {
  // u bez-curve
  Point A = bezCurvifier(bsCPoints[0], bsCPoints[1], bsCPoints[2], bsCPoints[3], u)[0];
  Point B = bezCurvifier(bsCPoints[4], bsCPoints[5], bsCPoints[6], bsCPoints[7], u)[0];
  Point C = bezCurvifier(bsCPoints[8], bsCPoints[9], bsCPoints[10], bsCPoints[11], u)[0];
  Point D = bezCurvifier(bsCPoints[12], bsCPoints[13], bsCPoints[14], bsCPoints[15], u)[0];
  // v bez-curve
  Point E = bezCurvifier(bsCPoints[0], bsCPoints[4], bsCPoints[8], bsCPoints[12], v)[0];
  Point F = bezCurvifier(bsCPoints[1], bsCPoints[5], bsCPoints[9], bsCPoints[13], v)[0];
  Point G = bezCurvifier(bsCPoints[2], bsCPoints[6], bsCPoints[10], bsCPoints[14], v)[0];
  Point H = bezCurvifier(bsCPoints[3], bsCPoints[7], bsCPoints[11], bsCPoints[15], v)[0];
  // (point, dP/dv) tuple
  std::vector<Point> vCurve = bezCurvifier(A, B, C, D, v);
  // (point, dP/du) tuple
  std::vector<Point> uCurve = bezCurvifier(E, F, G, H, u);

  // Normalized normal
  Point normal(uCurve[1].y*vCurve[1].z - uCurve[1].z*vCurve[1].y, 
	       uCurve[1].z*vCurve[1].x - uCurve[1].x*vCurve[1].z,
	       uCurve[1].x*vCurve[1].y - uCurve[1].y*vCurve[1].x);
  float mag = sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
  normal.x = normal.x / mag;
  normal.y = normal.y / mag;
  normal.z = normal.z / mag;
  

  // Result tuple (point, normal)
  std::vector<Point> result;
  result.push_back(vCurve[0]);
  result.push_back(normal);
  return result;
}

/** Create CURVE bezier point, given u and v of bezier curve control points */
std::vector<Point> bezCurvifier(Point p0, Point p1, Point p2, Point p3, float t) {
  // Order matters
  Point A(p0.x * (1.0 - t) + p1.x * t,
	  p0.y * (1.0 - t) + p1.y * t,
	  p0.z * (1.0 - t) + p1.z * t);

  Point B(p1.x * (1.0 - t) + p2.x * t,
	  p1.y * (1.0 - t) + p2.y * t,
	  p1.z * (1.0 - t) + p2.z * t);

  Point C(p2.x * (1.0 - t) + p3.x * t,
	  p2.y * (1.0 - t) + p3.y * t,
	  p2.z * (1.0 - t) + p3.z * t);

  //Now, split AB and BC to form a new segment DE:
  Point D(A.x * (1.0 - t) + B.x * t,
	  A.y * (1.0 - t) + B.y * t,
	  A.z * (1.0 - t) + B.z * t);

  Point E(B.x * (1.0 - t) + C.x * t,
	  B.y * (1.0 - t) + C.y * t,
	  B.z * (1.0 - t) + C.z * t);

  // Pick the right point on DE:
  Point finalP(D.x * (1.0 - t) + E.x * t,
	       D.y * (1.0 - t) + E.y * t,
	       D.z * (1.0 - t) + E.z * t);

  // Find dP/dt
  Point dPdt(3 * (E.x - D.x), 
	     3 * (E.y - D.y), 
	     3 * (E.z - D.z));

  // Result
  std::vector<Point> result;
  result.push_back(finalP);
  result.push_back(dPdt);

  return result;
}


/** 
 * NORMALKEYFUNC method specifies fill, wireframe, and shading
 options of the object. */
void normalKeyFunc(unsigned char key, int x, int y) {

  switch(key){
    // spacebar : closes window
  case 32:
    exit(0);
    break;

  case 's':
    isS = true;
    lightSetUp();
    /*toggle flat to smooth shading */
    if (isFlat) {
      glShadeModel(GL_SMOOTH);
      isFlat = false;
    } else {
      glShadeModel(GL_FLAT);
      isFlat = true;
    }
    break;
    // w : filled to wireframe
  case 'w':
    isS = false;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    if (isWireframe) {
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      isWireframe = false;
    } else {
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      isWireframe = true;
    }
    break;
  case '+':
    zoom += 0.1f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5.0 + zoom, 5.0 - zoom, -5.0 + zoom, 5.0 - zoom, 1.0, 30.0);
    break;
  case '-':
    zoom -= 0.1f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5.0 + zoom, 5.0 - zoom, -5.0 + zoom, 5.0 - zoom, 1.0, 30.0);
    break;
  }
  glutPostRedisplay();
}


/** 
 * SPECIALKEYFUNC method specifies the zoom, transformation.*/

void specialKeyFunc(int key, int x, int y) {
  //if shift is pressed:
  int gMods = glutGetModifiers();
  if (gMods == GLUT_ACTIVE_SHIFT) {
    switch(key){
      // left arrow : translate object
    case GLUT_KEY_LEFT :
      translateX -= 0.15f;
      break;
      // right arrow : translate object
    case GLUT_KEY_RIGHT :
      translateX += 0.15f;
      break;
    case GLUT_KEY_UP :
      translateY += 0.15f;
      break;
    case GLUT_KEY_DOWN :
      translateY -= 0.15f;
      break;
      
    }
  } else {
    switch(key){
      // left arrow : rotate object
    case GLUT_KEY_LEFT :
      rotateY += 5.0;
      break;
      // right arrow : rotate object
    case GLUT_KEY_RIGHT :
      rotateY -= 5.0;
      break;
    case GLUT_KEY_UP :
      rotateX += 5.0;
      break;
    case GLUT_KEY_DOWN :
      rotateX -= 5.0;
      break;
    }
  }
  glutPostRedisplay();
}


int main(int argc, char *argv[]) {
  /** File parsing: */

  // Parsing command line for: input file name, the subdivision paramter, and
  // the flag which determines if subdivision should be adaptive or uniform:
  std::string file = std::string(argv[1]);
  tesType = std::string(argv[3]);
  subStep = atof(argv[2]);

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
	numPatches.push_back(atoi(splitline[0].c_str())); 
	continue;
	//in the .scene file, each object is separated by a '# #'
	//so the '# #' is parsed, the patches for the object is added
	//to bigPatches, and patches gets cleared
      } else if(splitline.size() == 2) {
	bigPatches.push_back(patches);
	patches.clear();
	continue;

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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Initalize the viewport size
  viewport.w = 800;
  viewport.h = 800;

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
  glutKeyboardFunc(normalKeyFunc);
  glutSpecialFunc(specialKeyFunc);
  
  // Glut main infinite loop
  glutMainLoop();
}

