/*
Bomb class file

Chaos The Game
*/

#include "bomb.h"

#include <chrono>

using namespace std;


Bomb::Bomb(int x, int y) : posX(x), posY(y) {
	radius = 10.0;
	radius_step = 0.4;
	
	warning_mode = true;
	destroy = false;
	
	warning_time = 3.0;
	destroy_time = 10.0;
	
	start_time = chrono::system_clock::now();
}

// nothing was created that must be deallocated
Bomb::~Bomb() {

}

// updates the state of the bomb
// increases timer and possibly expands radius
void Bomb::update() {
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds;
	elapsed_seconds = current - start_time;

	if (!warning_mode) {
		// if the bomb has already detonated, grow the radius
		if ((int) radius != final_radius) {
			radius += radius_step;
		}
	} else {
		// if the bomb is still waiting to detonate, check the timer
		if (elapsed_seconds.count() > warning_time) {
			warning_mode = false;
		}
	}
	
	// destoy the bomb after it has existed for a given amount of time
	if (elapsed_seconds.count() > destroy_time) {
		destroy = true;
	}
}


