/*
Bomb class header file
An explosive that detonates after a timer expires and grows outward, eliminating any player it touches

Chaos The Game
*/

#ifndef BOMB_H
#define BOMB_H

#include <chrono>

using namespace std;


class Bomb {

public:
	Bomb(int x, int y);
	~Bomb();
	
	// position of the bomb
	int posX;
	int posY;
	
	// radius of the bomb
	double radius;
	// the amount to grow the radius each frame
	double radius_step;
	
	// indicates if the bomb is still a warning or if it has detonated
	bool warning_mode;
	
	// indicates to the bomb manager that the bomb is ready to be destroyed
	bool destroy;
	
	// maximum radius of the bomb
	static const int final_radius = 40;
	
	// the timer used for checking the age of the bomb
	chrono::time_point<chrono::system_clock> start_time;
	
	// time period that the bomb is waiting to detonate
	double warning_time;
	// the time after which to delete the bomb
	double destroy_time;
	
	// update the bomb each frame
	void update();

};

#endif
