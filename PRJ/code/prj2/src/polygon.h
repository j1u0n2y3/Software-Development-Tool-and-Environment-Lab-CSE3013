#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "ofMain.h"
#include <vector>
using namespace std;

struct POINT2D
{
    int x, y; /* coordinates of a point */
    POINT2D() : x(-1), y(-1) {}
    POINT2D(int X, int Y) : x(X), y(Y) {}
};

struct POLYGON
{
    vector<POINT2D> convexhull; /* points on a convex hull polygon */
    double area;                /* area */
    int r, g, b;                /* color */
    void set_convex_hull();
};

vector<POINT2D> convexHull(vector<POINT2D> &points);
double polygonArea(vector<POINT2D> &points);
bool isPointInsidePolygon(const vector<POINT2D> &polygon, const POINT2D &point);
bool onSegment(const POINT2D &p, const POINT2D &q, const POINT2D &r);
bool isSegmentsIntersect(const POINT2D &p1, const POINT2D &q1, const POINT2D &p2, const POINT2D &q2);
bool overlaps(const vector<POINT2D> &polygon1, const vector<POINT2D> &polygon2);
bool isPolygonInBlock(POLYGON polygon, int i, int j);

#endif /* _POLYGON_H_ */