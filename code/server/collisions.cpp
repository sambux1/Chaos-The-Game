/*
Collision handler class file
static methods to check for collisions between various objects

Chaos the Game
*/

#include "collisions.h"

#include "polygon.h"
#include "projectile.h"
#include "point_vect_struct.h"
#include "wall.h"

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

// checks for collision between a wall and a ball and adjusts the path of the ball
// will account for wall rotation later
bool Collisions::wall_ball_collision(Projectile* ball, Wall* wall) {
	
	// check for collisions with spikes
	
	for (Polygon polygon : wall->spikes) {
		for (int i1 = 0; i1 < polygon.points.size(); i1++) {
			int i2 = (i1 + 1) % polygon.points.size();
		
			point p1 = polygon.points[i1];
			point p2 = polygon.points[i2];
		
			point b((int) ball->posX, (int) ball->posY);
		
			if (line_circle_collision(p1, p2, ball)) {
				vect wall_vect((p2.x - p1.x), (p2.y - p1.y));
				return true;
			}
		}
	}
	
	// check for collision with rectangle
	
	for (int i1 = 0; i1 < wall->body.points.size(); i1++) {
		int i2 = (i1 + 1) % wall->body.points.size();
		
		point p1 = wall->body.points[i1];
		point p2 = wall->body.points[i2];
		
		point b((int) ball->posX, (int) ball->posY);
		
		if (line_circle_collision(p1, p2, ball)) {
			vect wall_vect((p2.x - p1.x), (p2.y - p1.y));
			calculate_deflection(ball, wall_vect);
			break;
		}
	}
	
	return false;
	
}

// find the intersectin point between a line and a point, passed as 3 points
/*
TODO: add description of this process
*/
// might be IRRELEVANT
point Collisions::calculate_intersection(point a, point b, point ball) {
	// the slope of the line from a to b
	double slope_line;
	if ((b.y - a.y) == 0) {
		point ret(ball.x, a.y);
		return ret;
	} else if ((b.x - a.x) == 0) {
		point ret(a.x, ball.y);
		return ret;
	} else {
		slope_line = ( (b.y - a.y) / (b.x - a.x) );
	}
	
	// the slope of the line normal to this is (-1 / slope_line)
	
	// calculate the x coordinate of the intersection
	double numerator = ( (ball.x / slope_line) + ball.y + (a.x * slope_line) - a.y );
	double denominator = (slope_line + (1 / slope_line) );
	double x = numerator / denominator;
	double y = (slope_line * (x - a.x)) + a.y;
	
	point ret((int) x, (int) y);
	return ret;
}

// determine if there is a collision between a line and a circle
bool Collisions::line_circle_collision(point a, point b, Projectile* ball) {
	// check if either endpoint of the line is inside the circle
	double distance_a = get_distance(a.x, a.y, ball->posX, ball->posY);
	double distance_b = get_distance(b.x, b.y, ball->posX, ball->posY);
	
	if ((distance_a <= ball->RADIUS) || (distance_b <= ball->RADIUS)) {
		// collision with endpoint, return true
		return true;
	}
	
	// calculate the length of the line
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	double len = sqrt((dx * dx) + (dy * dy));
	
	// create vectors from the circle to point a and from a to b
	vect circle_to_a((ball->posX - a.x), (ball->posY - a.y));
	vect line((b.x - a.x), (b.y - a.y));
	
	// calculate the dot product of the two vectors and divide by length squared
	double dot = dot_product(circle_to_a, line) / (len * len);
	
	// find the closest point on the line to the circle using the dot product
	point closest;
	closest.x = a.x + (dot * line.x);
	closest.y = a.y + (dot * line.y);
	
	// check if this point is within the line segment
	bool on_segment = is_within_segment(a, b, closest);
	if (!on_segment) {
		return false;
	}
	
	// find the distance from the circle to the closest point on the line
	double dist = get_distance(closest.x, closest.y, ball->posX, ball->posY);
	if (dist <= ball->RADIUS) {
		return true;
	} else {
		return false;
	}
}

// check if a point is within the bounding rectangle of a line segment
bool Collisions::is_within_segment(point line1, point line2, point p) {
	int xMin, xMax;
	if (line1.x >= line2.x) {
		xMax = line1.x;
		xMin = line2.x;
	} else {
		xMax = line2.x;
		xMin = line1.x;
	}
	
	int yMin, yMax;
	if (line1.y >= line2.y) {
		yMax = line1.y;
		yMin = line2.y;
	} else {
		yMax = line2.y;
		yMin = line1.y;
	}
	
	if ( (p.x >= xMin) && (p.x <= xMax) && (p.y >= yMin) && (p.y <= yMax) ) {
		return true;
	} else {
		return false;
	}
}

// when there is a collision between a ball and a wall, calculate the new path of the ball
/*
process:
	- calculate unit normal vector of the wall
	- find components of velocity vector parallel (w) and normal (u) to the wall
		u = (v dot n) * n
		w = v - u
	- set new ball velocity to v' = w - u
*/
void Collisions::calculate_deflection(Projectile* ball, vect wall_vect) {
	// find the unit normal vector of the wall
	vect n = calculate_unit_normal_vector(wall_vect);
	
	// create a velocity vector for the ball from its velocity components
	vect v(ball->velX, ball->velY);
	
	// calculate u
	double scalar = dot_product(v, n);
	vect u((n.x * scalar), (n.y * scalar));
	
	// calculate w
	vect w((v.x - u.x), (v.y - u.y));
	
	// calculate v'
	vect v_prime((w.x - u.x), (w.y - u.y));
	
	// set the ball's velocity to v'
	ball->velX = v_prime.x;
	ball->velY = v_prime.y;
	
	cout << "changing velocity vector!!" << endl;
}

// calculate the unit normal vector of a given vector
vect Collisions::calculate_unit_normal_vector(vect v) {
	// calculate normal (not unit)
	vect normal(-v.y, v.x);
	
	// calculate magnitude of normal
	double magnitude = sqrt( (normal.x * normal.x) + (normal.y * normal.y) );
	
	// divide normal by magnitude to get unit normal
	vect unit_normal;
	unit_normal.x = normal.x / magnitude;
	unit_normal.y = normal.y / magnitude;
	
	return unit_normal;
}


// helper function for getting distance
double Collisions::get_distance(double fromX, double fromY, double toX, double toY) {
	double dx = fromX - toX;
	double dy = fromY - toY;
	return sqrt((dx * dx) + (dy * dy));
}

double Collisions::dot_product(vect a, vect b) {
	return ( (a.x * b.x) + (a.y * b.y) );
}

