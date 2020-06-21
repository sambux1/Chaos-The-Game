/*
Player class header file

Chaos The Game
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "polygon.h"
#include "point_vect_struct.h"

using namespace std;

/*
Using a rectangle as a placeholder for the player
*/

class Player {

public:
	Player();
	~Player();
	
	// dimensions of the rectangle
	static const int PLAYER_WIDTH = 100;
	static const int PLAYER_HEIGHT = 100;
	
	// position of the center of the rectangle
	// stored as a double to allow for smooth motion, cast to an int to draw to the screen
	double posX;
	double posY;
	
	// velocity in the forward/backward direction
	int vel;
	// velocity in terms of game coordinates
	double velX;
	double velY;
	
	// angle of rotation (degrees)
	int rotation;
	// direction of rotation (clockwise/counterclockwise)
	int rotationVel;
	
	// the color of the player's rectangle
	string color;
	
	// the state of the player's projectile-firing ability
	// necessary to fire projectiles and ensure only one is fired for each key press
	bool shoot_projectile;
	bool ready_to_shoot;
	
	// calculate the corners of the rectangle
	void update_rectangle_points();
	
	// temp values used for collision checking
	double newX;
	double newY;
	int newRotation;
	
	// sets temp values to the actual position and rotation
	void reset_temp_vars();
	
	// the polygon used for collision checking
	Polygon body;

};

#endif

