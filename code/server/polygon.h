/*
Polygon class header file
Used for collision detection

Chaos The Game
*/

#ifndef POLYGON_H
#define POLYGON_H

#include "point_vect_struct.h"

#include <vector>

using namespace std;


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
