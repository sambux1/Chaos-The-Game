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
	Wall(int x, int y, int rot);
	~Wall();
	
	// the length of the wall
	static const int WALL_HEIGHT = 140;
	
	// coordinates of the center point
	int posX;
	int posY;
	
	// rotation of the wall (degrees)
	int rotation;
	int rotationVel;
	// the temp rotation used for collision checking
	int newRotation;
	// final rotation
	int target_rotation;
	
	// checks if the wall is able to rotate without causing a collision
	bool can_rotate;
	
	// calculate the coordinates of each point in the polygon
	void update_points();
	
	// the collision body
	Polygon body;

};

#endif

