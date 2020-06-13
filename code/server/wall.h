/*
Inner Wall class header file

Chaos The Game
*/

#ifndef WALL_H
#define WALL_H

#include "polygon.h"

#include <vector>


class Wall {

public:
	Wall(int x, int y);
	~Wall();
	
	// dimensions of the wall, likely to change
	static const int WALL_WIDTH = 40;
	static const int WALL_HEIGHT = 200;
	
	// length of the spike, forms an equilateral triangle
	static const int SPIKE_HEIGHT = 35; // WALL_WIDTH * sqrt(3) / 2
	
	// coordinates of the center point
	int posX;
	int posY;
	
	// rotation of the wall (degrees)
	int rotation;
	int rotationVel;
	
	// final rotation
	int final_rotation;
	
	// calculate the coordinates of each point in the polygon
	void update_points();
	
	// the two spikes on either end of the rectangle
	Polygon spike1;
	Polygon spike2;
	vector<Polygon> spikes;
	
	// the collision body
	Polygon body;

};

#endif
