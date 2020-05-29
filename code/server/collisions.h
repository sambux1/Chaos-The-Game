/*
Collision handler class header file
static methods to check for collisions between various objects

Chaos the Game
*/

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "polygon.h"
#include "projectile.h"

using namespace std;


class Collisions {

public:
	Collisions();
	~Collisions();
	
	static bool polygon_collision(Polygon a, Polygon b);
	static bool ball_player_collision(Projectile* ball, Polygon polygon);
	
	static double get_distance(double fromX, double fromY, double toX, double toY);

};

#endif
