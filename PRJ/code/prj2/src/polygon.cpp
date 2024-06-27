#include "polygon.h"
#include <algorithm>
#include <stack>
#include <cmath>

void POLYGON::set_convex_hull()
{
    /* Calculate points and area on convex hull. */
    convexhull = convexHull(convexhull);
    area = polygonArea(convexhull);
    /* Randomize color. */
    r = 255 - ofRandom(100);
    g = 255 - ofRandom(100);
    b = 255 - ofRandom(100);
}

POINT2D p0(-1, -1);

/* ccw - Function to determine the orientation of the triplet (a, b, c) */
int ccw(POINT2D a, POINT2D b, POINT2D c)
{
    /* Calculate the determinant of the matrix composed of the points' coordinates
     * and subtract the determinant of the matrix composed of the points' coordinates in the reverse order.
     */
    int op = a.x * b.y + b.x * c.y + c.x * a.y;
    op -= (a.y * b.x + b.y * c.x + c.y * a.x);
    /* If the determinant is positive, the triplet (a, b, c) forms a counter-clockwise turn. */
    if (op > 0)
        return 1;
    /* If the determinant is zero, the triplet (a, b, c) is collinear. */
    else if (op == 0)
        return 0;
    /* If the determinant is negative, the triplet (a, b, c) forms a clockwise turn. */
    else
        return -1;
}

/* distSq - Function to return the square of the distance between two points */
double distSq(POINT2D p1, POINT2D p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

/* compare - Function to compare two points with respect to the first point p0 */
bool compare(POINT2D p1, POINT2D p2)
{
    /* Calculate the orientation of the triplet (p0, p1, p2). */
    int o = ccw(p0, p1, p2);

    if (o == 0) /* If the points p1 and p2 are collinear with p0, */
        /* Return true if p2 is further from p0 than p1, otherwise false. */
        return (distSq(p0, p2) >= distSq(p0, p1));
    else /* Otherwise, */
        /* Return true if the orientation is counterclockwise (o == 1), otherwise false. */
        return (o == 1);
}

/* convexHull - Funtion to store the points of the convex hull in a vector. */
vector<POINT2D> convexHull(vector<POINT2D> &points)
{
    int n = points.size();
    if (n < 3)
        return {};

    /* Find the bottommost point. */
    int ymin = points[0].y, minn = 0;
    for (int i = 1; i < n; i++)
    {
        int y = points[i].y;

        /* Pick the bottom-most or chose the left most point in case of tie. */
        if ((y < ymin) || (ymin == y && points[i].x < points[minn].x))
            ymin = points[i].y, minn = i;
    }

    /* Place the bottom-most point at first position. */
    swap(points[0], points[minn]);

    /* Sort n-1 points with respect to the first point. */
    p0 = points[0];
    sort(points.begin() + 1, points.end(), compare);

    /* Create an empty stack and push first three points to it. */
    vector<POINT2D> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    hull.push_back(points[2]);

    /* Process remaining n-3 points. */
    for (int i = 3; i < n; i++)
    {
        /* Keep removing top while the angle formed
         * by points next-to-top, top, and points[i] makes a non-left turn.
         */
        while (hull.size() > 1 && ccw(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) != 1)
            hull.pop_back();
        hull.push_back(points[i]);
    }

    return hull;
}

/* polygonArea - Function to calculate the area of a polygon given its vertices. */
double polygonArea(vector<POINT2D> &points)
{
    int n = points.size();
    double area = 0.0;

    /* Loop through each vertex (except the first and last) */
    for (int i = 1; i < n - 1; i++)
        /* Calculate part of the determinant (shoelace formula) and add to the total area.
         * This part calculates the area of the triangle formed by points[0], points[i], and points[i+1]
         * and adds/subtracts it from the total area accordingly.
         */
        area += (points[i].x - points[0].x) * (points[i + 1].y - points[i].y) - (points[i].y - points[0].y) * (points[i + 1].x - points[i].x);
    /* The formula gives twice the actual area and might be negative depending on the vertices' order,
     * so take the absolute value and divide by 2 to get the actual area.
     */
    return abs(area / 2.0);
}

/* isPointInsidePolygon - Function to determine if a point is inside a polygon */
bool isPointInsidePolygon(const vector<POINT2D> &polygon, const POINT2D &point)
{
    bool inside = false;

    /* Loop through each edge of the polygon
     * (j tracks the previous vertex, starting from the last vertex.)
     */
    for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        /* Check if the point crosses the edge of the polygon
         * by using the ray-casting algorithm to count intersections.
         */
        /* Calculate the x-coordinate where the horizontal line through the point intersects the edge. */
        if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
            (point.x < (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
            inside = !inside;
    }
    return inside;
}

/* onSegment - Function to check if point q lies on line segment pr */
bool onSegment(const POINT2D &p, const POINT2D &q, const POINT2D &r)
{
    /* Check if q's coordinates are within the bounding box defined by p and r. */
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;

    return false;
}

/* isSegmentsIntersect - Function to check if two line segments intersect */
bool isSegmentsIntersect(const POINT2D &p1, const POINT2D &q1, const POINT2D &p2, const POINT2D &q2)
{
    /* Find the four orientations needed. */
    int o1 = ccw(p1, q1, p2);
    int o2 = ccw(p1, q1, q2);
    int o3 = ccw(p2, q2, p1);
    int o4 = ccw(p2, q2, q1);

    /* GENERAL CASE */
    if (o1 != o2 && o3 != o4)
        return true;

    /* SPECIAL CASES */
    /* p1, q1 and p2 are colinear and p2 lies on segment p1q1 */
    if (o1 == 0 && onSegment(p1, p2, q1))
        return true;
    if (o2 == 0 && onSegment(p1, q2, q1))
        return true;

    /* p2, q2 and p1 are colinear and p1 lies on segment p2q2 */
    if (o3 == 0 && onSegment(p2, p1, q2))
        return true;
    if (o4 == 0 && onSegment(p2, q1, q2))
        return true;

    return false;
}

/* overlaps - Functions to check if two polygon regions overlap */
bool overlaps(const vector<POINT2D> &polygon1, const vector<POINT2D> &polygon2)
{

    /* Check if any point of polygon1 is inside polygon2. */
    for (const POINT2D &point : polygon1)
    {
        if (isPointInsidePolygon(polygon2, point))
            return true;
    }

    /* Check if any point of polygon2 is inside polygon1. */
    for (const POINT2D &point : polygon2)
    {
        if (isPointInsidePolygon(polygon1, point))
            return true;
    }

    /* Check for edge intersection between the two polygons. */
    for (int i = 0; i < polygon1.size(); i++)
    {
        for (int j = 0; j < polygon2.size(); j++)
        {
            /* Calculate the next vertex index for the current edge in polygon. */
            int next_i = (i + 1) % polygon1.size();
            int next_j = (j + 1) % polygon2.size();

            /* Check if the current edges from polygon1 and polygon2 intersect. */
            if (isSegmentsIntersect(polygon1[i], polygon1[next_i], polygon2[j], polygon2[next_j]))
                return true;
        }
    }

    return false;
}

/* getBlockCorners - Functions that return the four vertices of a block */
vector<POINT2D> getBlockCorners(int i, int j)
{
    vector<POINT2D> corners(4);
    /* blockSize = 10; */
    int x1 = i * 10, y1 = j * 10;
    int x2 = (i + 1) * 10, y2 = (j + 1) * 10;
    corners[0] = POINT2D(x1, y1); /* Top-left corner */
    corners[1] = POINT2D(x2, y1); /* Top-right corner */
    corners[2] = POINT2D(x2, y2); /* Bottom-right corner */
    corners[3] = POINT2D(x1, y2); /* Bottom-left corner */
    return corners;
}

/* isPolygonInBlock - Function to check if the given polygon overlaps
 *                    with the block at grid position (i, j)
 */
bool isPolygonInBlock(POLYGON polygon, int i, int j)
{
    /* Get the corners of the block at grid position (i, j). */
    vector<POINT2D> blockCorners = getBlockCorners(i, j);
    /* Loop through each edge of the polygon's convex hull */
    for (size_t p = 0; p < polygon.convexhull.size(); p++)
    {
        /* Get the current vertex and the next vertex of the polygon. */
        POINT2D p1 = polygon.convexhull[p];
        POINT2D p2 = polygon.convexhull[(p + 1) % polygon.convexhull.size()];
        for (size_t b = 0; b < blockCorners.size(); b++)
        {
            /* Get the current vertex and the next vertex of the block. */
            POINT2D b1 = blockCorners[b];
            POINT2D b2 = blockCorners[(b + 1) % blockCorners.size()];
            /* Check if the current edge of the polygon intersects
             * with the current edge(corner) of the block.
             */
            if (isSegmentsIntersect(p1, p2, b1, b2))
                return true;
        }
    }
    return false;
}