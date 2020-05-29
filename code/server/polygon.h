/*
Polygon class header file
Used for collision detection

Chaos The Game
*/

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

using namespace std;


/*
struct to contain a single point (x and y coordinates)
*/
typedef struct point {
	point() {}
	point(int x, int y) : x(x), y(y) {}
	int x;
	int y;
} point;

/*
struct to contain a vector (x and y coordinates)
*/
typedef struct vect {
	vect() {}
	vect(int x, int y) : x(x), y(y) {}
	double x;
	double y;
} vect;


class Polygon {

public:
	
	Polygon();
	~Polygon();
	
	// list of points that makes up the polygons
	vector<point> points;
	
	// values that are necessary for rectangle collision checking
	point center;
	int rect_rot;
	int rect_width;
	int rect_height;

};

#endif
