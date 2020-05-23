/*
Player class file

Chaos The Game
*/

// include header file
#include "player.h"

// include dependencies
#include <iostream>
#include <string>

using namespace std;


// constructor, does not do anything yet
Player::Player() {
	// set positions to -1 to signal that they have not been set yet by the arena
	posX = -1;
	posY = -1;
	velX = 0;
	velY = 0;
}

// destructor, does not do anything yet
Player::~Player() {

}
