/*
Inner Wall class header file

Chaos The Game
*/

#include "wall.h"

#include "polygon.h"

#include <iostream>
#include <cmath>
#include <vector>

using namespace std;


Wall::Wall(int x, int y, int rot) : posX(x), posY(y) {
	rotation = rot;
	newRotation = rot;
	
	// set target rotation
	if (rot == 0) {
		target_rotation = 90;
		rotationVel = 1;
	} else {
		target_rotation = 0;
		rotationVel = -1;
	}
	
	// set rectangle dimensions for the collision box
	body.rect_height = WALL_HEIGHT;
	
	update_points();
}

// nothing to deallocate
Wall::~Wall() {

}

// update the position of the two endpoints
void Wall::update_points() {
	// calculate the unit vectors normal to the rectangle along both axes
	vect v1;
	v1.x = sin(newRotation * (M_PI / 180));
	v1.y = - cos(newRotation * (M_PI / 180));
	
	// scale both vectors
	v1.x *= WALL_HEIGHT / 2;
	v1.y *= WALL_HEIGHT / 2;
	
	// clear the points vector
	body.points.clear();
	
	point p1((int) (posX + v1.x + 0.5), (int) (posY + v1.y + 0.5));
	point p2((int) (posX - v1.x + 0.5), (int) (posY - v1.y + 0.5));
	body.points.push_back(p1);
	body.points.push_back(p2);
	
	// set other values for the collision box
	body.center.x = posX;
	body.center.y = posY;
	body.rect_rot = rotation;
}



