/*
Wall manager class header file
Creates and updattes walls separately from the arena file for cleanliness

Chaos the Game
*/

#ifndef WALL_MANAGER_H
#define WALL_MANAGER_H

#include "wall.h"

#include <set>

using namespace std;


class Wall_Manager {

public:
	Wall_Manager();
	~Wall_Manager();
	
	void create_walls();
	
	set<Wall*> walls;

};

#endif
