/*
Collision handler class file
static methods to check for collisions between various objects

Chaos the Game

******************
move polygon-polygon collisions here
******************
*/

#include "collisions.h"

#include "polygon.h"
#include "projectile.h"

#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

using namespace std;


Collisions::Collisions() {

}

Collisions::~Collisions() {

}

// check for collision between two polygons
bool Collisions::polygon_collision(Polygon a, Polygon b) {
	vector<Polygon> polygons = {a, b};
	
	for (Polygon polygon : polygons) {
		// iterate over each point and connect to the next point to check each edge
		for (int i1 = 0; i1 < polygon.points.size(); i1++) {
			int i2 = (i1 + 1) % polygon.points.size();
			
			point p1 = polygon.points[i1];
			point p2 = polygon.points[i2];
			
			// find the vector normal to the edge
			vect normal(p2.y - p1.y, p1.x - p2.x);
			
			// find the projection of each point in both polygons onto the normal vector
			double minA = numeric_limits<double>::max();
			double maxA = numeric_limits<double>::min();
			
			for (point p : a.points) {
				double projection = (normal.x * p.x) + (normal.y * p.y);
				
				if (projection < minA) {
					minA = projection;
				}
				if (projection > maxA) {
					maxA = projection;
				}
			}
			
			
			double minB = numeric_limits<double>::max();
			double maxB = numeric_limits<double>::min();
			
			for (point p : b.points) {
				double projection = (normal.x * p.x) + (normal.y * p.y);
				
				if (projection < minB) {
					minB = projection;
				}
				if (projection > maxB) {
					maxB = projection;
				}
			}
			
			// if there is no overlap, separating axis has been found, no collision
			if (maxA < minB || maxB < minA) {
				return false;
			}
		}
	}
	
	// if it reaches here, then no separating axis was found, meaning there is a collision
	return true;
}

// check for a collision between a ball and a player
bool Collisions::ball_player_collision(Projectile* ball, Polygon polygon) {
	/*
	add check for distance between the two objects to see if further calculations are necessary
	*/
	
	// find the location of the point after the entire frame has been rotated
	point unrotated_circle;
	unrotated_circle.x = (cos(polygon.rect_rot * (M_PI / 180)) * (ball->posX - polygon.center.x)) -
						 (sin(polygon.rect_rot * (M_PI / 180)) * (ball->posY - polygon.center.y)) +
						 polygon.center.x;
	unrotated_circle.y = (sin(polygon.rect_rot * (M_PI / 180)) * (ball->posX - polygon.center.x)) +
						 (cos(polygon.rect_rot * (M_PI / 180)) * (ball->posY - polygon.center.y)) +
						 polygon.center.y;
	
	// calculate a reference point on the rectangle
	point rect_ref;
	rect_ref.x = polygon.center.x - (polygon.rect_width / 2);
	rect_ref.y = polygon.center.y - (polygon.rect_height / 2);
	
	// find the closest point on the rectangle to the ball
	int closestX, closestY;
	
	if (unrotated_circle.x < rect_ref.x) {
		closestX = rect_ref.x;
	} else if (unrotated_circle.x > (rect_ref.x + polygon.rect_width)) {
		closestX = rect_ref.x + polygon.rect_width;
	} else {
		closestX = unrotated_circle.x;
	}
	
	if (unrotated_circle.y < rect_ref.y) {
		closestY = rect_ref.y;
	} else if (unrotated_circle.y > (rect_ref.y + polygon.rect_height)) {
		closestY = rect_ref.y + polygon.rect_height;
	} else {
		closestY = unrotated_circle.y;
	}
	
	int distance = (int) (get_distance(closestX, closestY, unrotated_circle.x, unrotated_circle.y) + 0.5);
	if (distance < Projectile::RADIUS) {
		return true;
	}
	
	// if it reaches here, then the projectile was not within 10 pixels of the polygon, so no collision
	return false;
}

// helper function for getting distance
double Collisions::get_distance(double fromX, double fromY, double toX, double toY) {
	double dx = fromX - toX;
	double dy = fromY - toY;
	return sqrt((dx * dx) + (dy * dy));
}









