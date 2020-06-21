/*
Wall manager class header file
Creates and updattes walls separately from the arena file for cleanliness

Chaos the Game
*/

#ifndef WALL_MANAGER_H
#define WALL_MANAGER_H

#include "wall.h"

#include <set>
#include <chrono>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;


class Wall_Manager {

public:
	Wall_Manager();
	~Wall_Manager();
	
	void start();
	void create_walls();
	void update_walls();
	
	// free memory and prepare for the arena to be reset
	void clean_up();
	
	set<Wall*> walls;
	// sets for the 3 states a wall can be in
	set<Wall*> unrotated_walls;
	set<Wall*> rotating_walls;
	set<Wall*> finished_rotating_walls;
	
	chrono::time_point<chrono::system_clock> start_time;
	
	double waiting_time;

};

#endif

