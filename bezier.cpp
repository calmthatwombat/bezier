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
void subdividePatch (std::vector<Point> patch, float step);
Point bezSurfacifier(std::vector<Point> bsCPoints, float u, float v);
Point bezCurvifier(Point p0, Point p1, Point p2, Point p3, float t);
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
int numPatches;
std::vector< std::vector<Point> > patches; // control points
float subStep;
std::string tesType;
std::vector< std::vector<Point> > unifPatches; // bezier surface points

//Lizzie added these for transformations//////////////////////////
float zoom = 0.0;
float rotateX = 0.0;
float rotateY = 0.0;
float translateX = 0.0;
float translateY = 0.0;

//Lizzie added for shading and wireframe modes////////////////////
bool isWireframe;
bool isFlat;



/** Glut display method, loads identity matrix and draws */
void myDisplay() {

  lightSetUp();

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Indicate we are specifying camera transformations
  glMatrixMode(GL_MODELVIEW);
  // Zero the first transformation
  glLoadIdentity();

  for (int i = 0; i < patches.size(); i++) {
    subdividePatch(patches[i], subStep);
  }

  for (int i = 0; i < unifPatches.size(); i++) {

  }

  //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  //Camera Transformations
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //glOrtho(-1.0 + zoom, 1.0 - zoom, -1.0 + zoom, 1.0 - zoom, 1.0, -1.0);

  glTranslatef(translateX, translateY, 0.0f);
  glRotatef(rotateX, 1.0, 0.0, 0.0 );
  glRotatef(rotateY, 0.0, 1.0, 0.0 );
  //glScalef(zoom, zoom, zoom);////



  //3d cube
  // White side - BACK
  glBegin(GL_QUADS);
  glColor3f(   1.0,  1.0, 1.0 );
  glVertex3f(  0.5, -0.5, 0.5 );
  glVertex3f(  0.5,  0.5, 0.5 );
  glVertex3f( -0.5,  0.5, 0.5 );
  glVertex3f( -0.5, -0.5, 0.5 );
  glEnd();

// Purple side - RIGHT
  glBegin(GL_QUADS);
  glColor3f(  1.0,  0.0,  1.0 );
  glVertex3f( 0.5, -0.5, -0.5 );
  glVertex3f( 0.5,  0.5, -0.5 );
  glVertex3f( 0.5,  0.5,  0.5 );
  glVertex3f( 0.5, -0.5,  0.5 );
  glEnd();

// Green side - LEFT
  glBegin(GL_QUADS);
  glColor3f(   0.0,  1.0,  0.0 );
  glVertex3f( -0.5, -0.5,  0.5 );
  glVertex3f( -0.5,  0.5,  0.5 );
  glVertex3f( -0.5,  0.5, -0.5 );
  glVertex3f( -0.5, -0.5, -0.5 );
  glEnd();

// Blue side - TOP
  glBegin(GL_QUADS);
  glColor3f(   0.0,  0.0,  1.0 );
  glVertex3f(  0.5,  0.5,  0.5 );
  glVertex3f(  0.5,  0.5, -0.5 );
  glVertex3f( -0.5,  0.5, -0.5 );
  glVertex3f( -0.5,  0.5,  0.5 );
  glEnd();

// Red side - BOTTOM
  glBegin(GL_QUADS);
  glColor3f(   1.0,  0.0,  0.0 );
  glVertex3f(  0.5, -0.5, -0.5 );
  glVertex3f(  0.5, -0.5,  0.5 );
  glVertex3f( -0.5, -0.5,  0.5 );
  glVertex3f( -0.5, -0.5, -0.5 );
  glEnd();

  
  
  

  glFlush();
  // We earlier set double buffers
  glutSwapBuffers();
}

/** Sets up the camera for transformations */
void cameraSetUp() {
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  // glTranslatef(0.0f, 0.0f, zoom);
  // glTranslatef(translateX, translateY, 0.0f);
  // glRotatef(rotateX, 1.0, 0.0, 0.0 );
  // glRotatef(rotateY, 0.0, 1.0, 0.0 );

  glMatrixMode(GL_PROJECTION);

  glOrtho(-1.0 + zoom, 1.0 - zoom, -1.0 + zoom, 1.0 - zoom, 1.0, -1.0);


}

/** Sets up the lights and light properties in the scene. */
void LightSetUp() {

  // defining light attributes:
  GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
  GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f };

  //creating and making light0 with the above attributes:
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);


  //enabling the lighting
  // glEnable(GL_LIGHTING);
  // glEnable(GL_LIGHT0);

}



/** Reshapes viewport if dragged and reshaped */
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;
  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //gluOrtho2D(0, viewport.w, 0, viewport.h);
  glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch
}

/** Simple scene initialization */
void initScene(){

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
  myReshape(viewport.w,viewport.h);

  //Clear the depth buffer///////////////////////////////
  glClearDepth(1);

  //Enables depth testing (for lights)///////////////////////////////
  glEnable(GL_DEPTH_TEST);

}

/** Given a patch containing 16 control points, push back a patch of (numDiv + 1)**2 
 *  surface points into unifPatches vector. */
void subdividePatch (std::vector<Point> patch, float step) {
  // current patch
  std::vector<Point> currPatch;
  
  //setting the epsilon value:
  float epsilon = 0.000001;

  //the parametric values u, v
  float u;
  float v;

  //finding the number of divisions:
  int numDiv = ((1 + epsilon) / step);

  //for each parametric value of u:
  for (float iu = 0; iu <= numDiv; iu++ ) {
    u = iu * step;
    //for each parametric value of v:
    for (float iv = 0; iv <= numDiv; iv++) {
      v = iv * step;
      // Calculate point
      currPatch.push_back(bezSurfacifier(patch, u, v));
    }
  }
  // Push this patch onto unifPatches
  unifPatches.push_back(currPatch);
}

/** Create SURFACE bezier point, given u and v of bezier surface control points 
 *  bsCPoints == bezier surface Control Points */
Point bezSurfacifier(std::vector<Point> bsCPoints, float u, float v) {
  Point A = bezCurvifier(bsCPoints[0], bsCPoints[1], bsCPoints[2], bsCPoints[3], u);
  Point B = bezCurvifier(bsCPoints[4], bsCPoints[5], bsCPoints[6], bsCPoints[7], u);
  Point C = bezCurvifier(bsCPoints[8], bsCPoints[9], bsCPoints[10], bsCPoints[11], u);
  Point D = bezCurvifier(bsCPoints[12], bsCPoints[13], bsCPoints[14], bsCPoints[15], u);
  return bezCurvifier(A, B, C, D, v);
}

/** Create CURVE bezier point, given u and v of bezier curve control points */
Point bezCurvifier(Point p0, Point p1, Point p2, Point p3, float t) {
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

  //Finally, picking the right point on DE:
  Point finalP(D.x * (1.0 - t) + E.x * t,
	       D.y * (1.0 - t) + E.y * t,
	       D.z * (1.0 - t) + E.z * t);
  return finalP;
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
    // s : flat to smooth shading
    case 's':
      /*toggle flat to smooth shading */
      if (isFlat) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH);
        isFlat = false;
      } else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_FLAT);
        isFlat = true;
      }
      break;
    // w : filled to wireframe
    case 'w':
      if (isWireframe) {
        glDisable(GL_LIGHTING);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        isWireframe = false;
      } else {
        glDisable(GL_LIGHTING);
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        isWireframe = true;
      }
      break;
    case '+':
      zoom += 0.1f;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-1.0 + zoom, 1.0 - zoom, -1.0 + zoom, 1.0 - zoom, 1.0, -1.0);
      break;
    case '-':
      zoom -= 0.1f;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-1.0 + zoom, 1.0 - zoom, -1.0 + zoom, 1.0 - zoom, 1.0, -1.0);
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
      // left arrow : rotate object
      case GLUT_KEY_LEFT :
        translateX -= 0.15f;
        break;
      // right arrow : rotate object
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
  glutKeyboardFunc(normalKeyFunc);
  glutSpecialFunc(specialKeyFunc);
  
  // Glut main infinite loop
  glutMainLoop();
}

