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


  
