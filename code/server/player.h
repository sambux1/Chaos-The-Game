/*
Player class header file

Chaos The Game
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

/*
Using a rectangle as a placeholder for the player
*/

class Player {

public:
	Player();
	~Player();
	
	// dimensions of the rectangle
	static const int PLAYER_WIDTH = 150;
	static const int PLAYER_HEIGHT = 100;
	
	// position of the top left corner of the rectangle
	int posX;
	int posY;
	// velocity
	int velX;
	int velY;
	
	// the color of the player's rectangle
	string color;

private:

};

#endif
