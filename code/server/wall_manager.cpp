/*
Wall manager class file
Creates and updattes walls separately from the arena file for cleanliness

Chaos the Game
*/

#include "wall_manager.h"

#include "wall.h"

#include <set>
#include <chrono>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Wall_Manager::Wall_Manager() {
	waiting_time = 2.0;
}

Wall_Manager::~Wall_Manager() {
	// if the game is suddenly shut down, deallocate the memory used for walls
	clean_up();
}

// called when the arena starts to create the walls and start the timier
void Wall_Manager::start() {
	start_time = chrono::system_clock::now();
	create_walls();
}

void Wall_Manager::create_walls() {
	// bottom walls
	Wall* w1 = new Wall(280, 500, 0);
	Wall* w2 = new Wall(480, 500, 0);
	Wall* w3 = new Wall(680, 500, 0);
	walls.insert(w1);
	walls.insert(w2);
	walls.insert(w3);
	
	// top walls
	Wall* w4 = new Wall(280, 140, 0);
	Wall* w5 = new Wall(480, 140, 0);
	Wall* w6 = new Wall(680, 140, 0);
	walls.insert(w4);
	walls.insert(w5);
	walls.insert(w6);
	
	// side walls
	Wall* w7 = new Wall(180, 320, 90);
	Wall* w8 = new Wall(780, 320, 90);
	walls.insert(w7);
	walls.insert(w8);
	
	for (Wall* wall : walls) {
		unrotated_walls.insert(wall);
	}
}

// update each wall
void Wall_Manager::update_walls() {
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds;
	elapsed_seconds = current - start_time;
	
	// every set amount of time, start rotating another wall
	if (unrotated_walls.size() != 0) {
		if (elapsed_seconds.count() > waiting_time) {
			srand(time(NULL));
			int index = rand() % unrotated_walls.size();
			set<Wall*>::iterator itr = unrotated_walls.begin();
			advance(itr, index);
			Wall* wall = *itr;
			
			// move the wall to the rotating group
			unrotated_walls.erase(wall);
			rotating_walls.insert(wall);
			
			// update the timer
			start_time += chrono::milliseconds((int) waiting_time * 1000);
		}
	}
	
	// rotate walls
	for (Wall* wall : rotating_walls) {
		if (wall->can_rotate) {
			wall->rotation = wall->newRotation;
		}
		
		// check if the wall has reached its target rotation
		if (wall->rotation == wall->target_rotation) {
			rotating_walls.erase(wall);
			finished_rotating_walls.insert(wall);
		}
	}
}

// free memory and prepare for the arena to be reset
void Wall_Manager::clean_up() {
	// empty the three sets of walls
	// do not delete the walls yet, they will all be deleted from the set of all walls
	unrotated_walls.clear();
	rotating_walls.clear();
	finished_rotating_walls.clear();
	
	// remove the walls from the set and delete the walls
	for (Wall* wall : walls) {
		walls.erase(wall);
		delete wall;
	}
}





