/*
Player class file

Chaos The Game
*/

// include header file
#include "player.h"

#include "polygon.h"
#include "point_vect_struct.h"

#include <iostream>
#include <string>
#include <cmath>

using namespace std;


// constructor, does not do anything yet
Player::Player() {
	// set positions to -1 to signal that they have not been set yet by the arena
	posX = -1;
	posY = -1;
	
	// set velocity to 0
	vel = 0;
	velX = 0.0;
	velY = 0.0;
	
	// set rotation to 0
	rotation = 0;
	rotationVel = 0;
	
	// set rectangle dimensions for the collision box
	body.rect_width = PLAYER_WIDTH;
	body.rect_height = PLAYER_HEIGHT;
	
	// the player should be ready to shoot at the start of the game
	shoot_projectile = false;
	ready_to_shoot = true;
}

// destructor, nothing to deallocate
Player::~Player() {

}

// calculate the corners of the rectangle
void Player::update_rectangle_points() {
	// calculate the unit vectors normal to the rectangle along both axes
	vect v1;
	v1.x = sin(newRotation * (M_PI / 180));
	v1.y = - cos(newRotation * (M_PI / 180));
	// second vector is perpendicular to the first
	vect v2;
	v2.x = v1.y;
	v2.y = -v1.x;
	
	// scale both vectors
	v1.x *= PLAYER_HEIGHT / 2;
	v1.y *= PLAYER_HEIGHT / 2;
	v2.x *= PLAYER_WIDTH / 2;
	v2.y *= PLAYER_WIDTH / 2;
	
	// clear the points vector
	body.points.clear();
	
	// calculate the corners by moving the center by the normal vectors
	point p1((int) (newX + v1.x + v2.x + 0.5), (int) (newY + v1.y + v2.y + 0.5));
	body.points.push_back(p1);
	
	point p2((int) (newX - v1.x + v2.x + 0.5), (int) (newY - v1.y + v2.y + 0.5));
	body.points.push_back(p2);
	
	point p3((int) (newX - v1.x - v2.x + 0.5), (int) (newY - v1.y - v2.y + 0.5));
	body.points.push_back(p3);
	
	point p4((int) (newX + v1.x - v2.x + 0.5), (int) (newY + v1.y - v2.y + 0.5));
	body.points.push_back(p4);
	
	// set other values for the collision box
	body.center.x = newX;
	body.center.y = newY;
	body.rect_rot = newRotation;
}

// resets the temporary variables used for collision checking to the actual values of the player
void Player::reset_temp_vars() {
	newX = posX;
	newY = posY;
	newRotation = rotation;
}


