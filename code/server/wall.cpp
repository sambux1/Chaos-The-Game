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


Wall::Wall(int x, int y) : posX(x), posY(y) {
	rotation = 45;
	rotationVel = 0;
	update_points();
}

Wall::~Wall() {
	// nothing to do yet
}

void Wall::update_points() {
	// calculate the unit vectors normal to the rectangle along both axes
	vect v1;
	v1.x = sin(rotation * (M_PI / 180));
	v1.y = - cos(rotation * (M_PI / 180));
	// second vector is perpendicular to the first
	vect v2;
	v2.x = v1.y;
	v2.y = -v1.x;
	
	// used later for finding spike points
	vect spike_vect(v1.x, v2.x);
	
	// scale both vectors
	v1.x *= WALL_HEIGHT / 2;
	v1.y *= WALL_HEIGHT / 2;
	v2.x *= WALL_WIDTH / 2;
	v2.y *= WALL_WIDTH / 2;
	
	// clear the points vector
	body.points.clear();
	
	// calculate the corners by moving the center by the normal vectors
	point p1((int) (posX + v1.x + v2.x + 0.5), (int) (posY + v1.y + v2.y + 0.5));
	body.points.push_back(p1);
	
	point p2((int) (posX - v1.x + v2.x + 0.5), (int) (posY - v1.y + v2.y + 0.5));
	body.points.push_back(p2);
	
	point p3((int) (posX - v1.x - v2.x + 0.5), (int) (posY - v1.y - v2.y + 0.5));
	body.points.push_back(p3);
	
	point p4((int) (posX + v1.x - v2.x + 0.5), (int) (posY + v1.y - v2.y + 0.5));
	body.points.push_back(p4);
	
	// set other values for the collision box
	body.center.x = posX;
	body.center.y = posY;
	body.rect_rot = rotation;
	
	/*
	calculate the positions of the spikes
	done by extending v1 by the length of the spike
	*/
	
	spike_vect.x *= ( (WALL_HEIGHT / 2) + SPIKE_HEIGHT);
	spike_vect.y *= ( (WALL_HEIGHT / 2) + SPIKE_HEIGHT);
	
	
	// spike1
	spike1.points.clear();
	
	point s1((int) (posX + spike_vect.x + 0.5), (int) (posY + spike_vect.y + 0.5));
	spike1.points.push_back(s1);
	// points p1 and p4 are part of this spike
	spike1.points.push_back(p1);
	spike1.points.push_back(p4);
	
	
	// spike2
	spike2.points.clear();
	
	point s2((int) (posX + spike_vect.x + 0.5), (int) (posY + spike_vect.y + 0.5));
	spike2.points.push_back(s2);
	// points p2 and p3 are part of this spike
	spike2.points.push_back(p2);
	spike2.points.push_back(p3);
	
	spikes.clear();
	spikes.push_back(spike1);
	spikes.push_back(spike2);
	
	/*
	cout << "x: " << p1.x << ", y: " << p1.y << endl;
	cout << "x: " << p2.x << ", y: " << p2.y << endl;
	cout << "x: " << p3.x << ", y: " << p3.y << endl;
	cout << "x: " << p4.x << ", y: " << p4.y << endl;
	*/
}

/*
int main() {
	Wall w(200, 200);
	w.rotation = 0;
	w.update_points();
}
*/
