/*
Projectile class header file

Chaos The Game
*/

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>

using namespace std;


class Projectile {

public:
	Projectile(double shooterX, double shooterY, int shooterRot, int shooterHeight);
	~Projectile();
	
	// the radius of each ball
	static const int RADIUS = 10;
	
	// the speed with which projectiles will move
	static const int PROJECTILE_SPEED = 10;
	
	// the coordinates of the center point
	double posX;
	double posY;
	
	// the velocity of the ball
	double velX;
	double velY;
	
	// the player that shot the projectile (identified by color)
	string shooter_color;
	
	// number of frames the bullet has been active
	// necessary for expiring old bullets and avoiding killing the shooter when firing
	int tick_count;


};

#endif
