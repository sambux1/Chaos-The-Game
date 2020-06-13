/*
Wall manager class file
Creates and updattes walls separately from the arena file for cleanliness

Chaos the Game
*/

#include "wall_manager.h"

#include "wall.h"

#include <set>


Wall_Manager::Wall_Manager() {

}

Wall_Manager::~Wall_Manager() {

}

void Wall_Manager::create_walls() {
	Wall* w1 = new Wall(210, 500);
	Wall* w2 = new Wall(480, 500);
	Wall* w3 = new Wall(750, 500);
	walls.insert(w1);
	walls.insert(w2);
	walls.insert(w3);
	
	Wall* w4 = new Wall(210, 140);
	Wall* w5 = new Wall(480, 140);
	Wall* w6 = new Wall(750, 140);
	walls.insert(w4);
	walls.insert(w5);
	walls.insert(w6);
}
