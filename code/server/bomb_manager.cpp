/*
Bomb manager class file
Creates and updattes bombs separately from the arena file for cleanliness

Chaos the Game
*/

#include "bomb_manager.h"

#include "bomb.h"

#include <set>
#include <chrono>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Bomb_Manager::Bomb_Manager() {
	waiting_time = 3.0;
}

Bomb_Manager::~Bomb_Manager() {
	// if the arena is suddenly shut down, free the memory of the bombs
	clean_up();
}

// start the timer
void Bomb_Manager::start() {
	start_time = chrono::system_clock::now();
}

// update each bomb and possibly create or destroy bombs
void Bomb_Manager::update_bombs() {
	for (Bomb* bomb : bombs) {
		bomb->update();
		if (bomb->destroy) {
			bombs.erase(bomb);
			delete bomb;
		}
	}
	
	chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds;
	elapsed_seconds = current - start_time;
	
	/*
	will generate coordinates of a bomb and map the coordinates to a point in the outer ring
	*/
	
	if (elapsed_seconds.count() > waiting_time) {
		int rangeX = 680;
		int rangeY = 500;
		
		srand(time(NULL));
		int x = rand() % rangeX + 140;
		int y = rand() % rangeY + 70;
		// map coordinates to outer ring, start by finding distance in each direction
		// outer ring is at x = 90, x = 870, y = 70, y = 570
		int dx = min(abs(x - 90), abs(x - 870));
		int dy = min(abs(y - 70), abs(y - 570));
		
		if (dx < dy) {
			if (x > 480) {
				x += dx;
			} else {
				x -= dx;
			}
		} else {
			if (y > 320) {
				y += dy;
			} else {
				y -= dy;
			}
		}
		
		Bomb* bomb = new Bomb(x, y);
		bombs.insert(bomb);
		
		// update the timer
		start_time += chrono::milliseconds((int) waiting_time * 1000);
	}
}

// prepare for the arena to be reset and free memory
void Bomb_Manager::clean_up() {
	// remove each bomb from the set and delete the bomb object
	for (Bomb* bomb : bombs) {
		bombs.erase(bomb);
		delete bomb;
	}
}


