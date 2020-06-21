/*
Collision handler class header file
static methods to check for collisions between various objects

Chaos the Game
*/

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "polygon.h"
#include "projectile.h"
#include "point_vect_struct.h"
#include "wall.h"
#include "bomb.h"

using namespace std;


class Collisions {

public:
	Collisions();
	~Collisions();
	
	// check for collision between two polygons using separating axis theorem
	static bool polygon_collision(Polygon a, Polygon b);
	
	// check for collision between ball and wall
	static int wall_ball_collision(Projectile* ball, Wall* wall, bool deflect);
	
	// calculate the new velocity vector of a ball after colliding with a wall
	static void calculate_deflection(Projectile* ball, vect wall_vect);
	
	// check for collision between ball and player, calls circle_player_collision
	static bool ball_player_collision(Projectile* ball, Polygon polygon);
	
	// check for collision between bomb and player, calls circle_player_collision
	static bool bomb_player_collision(Bomb* bomb, Polygon polygon);
	
	// check for collision between circle (projectile or bomb) and player
	static bool circle_player_collision(point circle, int radius, Polygon polygon);
	
	// check for collision between a line and a ball
	static int line_circle_collision(point a, point b, Projectile* ball);
	
	// check if a point is within the bounding rectangle of a line segment
	static bool is_within_segment(point line1, point line2, point p);
	
	// calculate the distance between two points
	static double get_distance(double fromX, double fromY, double toX, double toY);
	
	// calculate the dot product of two vectors
	static double dot_product(vect a, vect b);
	
	// calculate the unit normal vector of a given vector
	static vect calculate_unit_normal_vector(vect v);

};

#endif
