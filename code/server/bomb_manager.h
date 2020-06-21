/*
Bomb manager class header file
Creates and updattes bombs separately from the arena file for cleanliness

Chaos the Game
*/

#ifndef BOMB_MANAGER_H
#define BOMB_MANAGER_H

#include "bomb.h"

#include <set>
#include <chrono>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;


class Bomb_Manager {

public:
	Bomb_Manager();
	~Bomb_Manager();
	
	// start the timer and start creating bombs
	void start();
	// update each bomb in the set
	void update_bombs();
	
	// free memory and prepare for the arena to be reset
	void clean_up();
	
	set<Bomb*> bombs;
	
	chrono::time_point<chrono::system_clock> start_time;
	
	// the amount of time to wait between creating bombs
	double waiting_time;

};

#endif

