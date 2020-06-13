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

using namespace std;


class Collisions {

public:
	Collisions();
	~Collisions();
	
	static bool polygon_collision(Polygon a, Polygon b);
	static bool ball_player_collision(Projectile* ball, Polygon polygon);
	static bool wall_ball_collision(Projectile* ball, Wall* wall);
	static void calculate_deflection(Projectile* ball, vect wall_vect);
	static vect calculate_unit_normal_vector(vect v);
	
	static bool line_circle_collision(point a, point b, Projectile* ball);
	static bool is_within_segment(point line1, point line2, point p);
	
	// find the intersectin point between a line and a point, passed as 3 points
	static point calculate_intersection(point a, point b, point ball);
	
	static double get_distance(double fromX, double fromY, double toX, double toY);
	static double dot_product(vect a, vect b);

};

#endif
