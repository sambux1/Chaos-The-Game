/*
point struct and vector struct
*/

#ifndef POINT_VECT_STRUCT_H
#define POINT_VECT_STRUCT_H

/*
struct to contain a single point (x and y coordinates)
*/
typedef struct point {
	point() {}
	point(int x, int y) : x(x), y(y) {}
	int x;
	int y;
} point;

/*
struct to contain a vector (x and y coordinates)
*/
typedef struct vect {
	vect() {}
	vect(double x, double y) : x(x), y(y) {}
	double x;
	double y;
} vect;

#endif
