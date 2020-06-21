/*
Arena class file

Chaos The Game

An arena is one game instance. It performs all actions associated with the
single game instance.
*/


// include header file
#include "arena.h"

// include game files
#include "player.h"
#include "message_struct.h"
#include "polygon.h"
#include "projectile.h"
#include "collisions.h"
#include "wall.h"
#include "wall_manager.h"
#include "bomb.h"

// include other dependencies
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <cmath>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// include boost string splitter
#include <boost/algorithm/string.hpp>

using namespace std;

// the colors that will be assigned to the players
const string Arena::color_list[] = {"blue", "green", "purple", "orange"};


// constructor
Arena::Arena() {
	num_players = 0;
	accepting_players = true;
	ready_to_start = false;
	color_index = 0;
	ready_to_reset = false;
}

// destructor
Arena::~Arena() {
	// if the arena shuts down unexpectedly, follow the standard protocol for cleaning
	clean_up();
}

// attempt to add a player to the arena, returns true if successful
bool Arena::add_player(Player* player) {
	// lock the arena lock to add a player to set
	lock_guard<mutex> guard(arena_lock);
	
	// check if the arena has stopped accepting players
	if (!accepting_players) {
		return false;
	}
	
	// game is still accepting players, add player to set and check for start condition
	arena_players.insert(player);
	num_players++;
	
	// assign the player the next color in the list and increment the index
	player->color = color_list[color_index];
	color_index++;
	
	// check if the arena should be closed off to new players
	if (num_players >= MAX_PLAYERS) {
		accepting_players = false;
	}
	
	// if the function reaches this point, the addition was successful
	return true;
}

// remove a player who has disconnected
void Arena::remove_player(Player* player) {
	// try to erase from each set (alive and dead)
	arena_players.erase(player);
	dead_players.erase(player);
}

// add a new message from the server to the queue of messages to be processed
void Arena::add_to_incoming_queue(message_struct* new_message) {
	// lock the arena to add to the queue
	lock_guard<mutex> guard(arena_lock);
	// push the message to the queue
	incoming_queue.push(new_message);
}

// add a new message produced by the arena to the queue to be sent
void Arena::add_to_outgoing_queue(string message) {
	// lock the arena to add to the queue
	lock_guard<mutex> guard(arena_lock);
	// push the message to the queue
	outgoing_queue.push(message);
}

// runs the processes to get the game ready to start, then runs the game loop
void Arena::start() {
	// when the arena is restarted, signal that it should not be reset again by gameserver
	ready_to_reset = false;
	
	// sets a timer for how long the arena can be waiting for players without starting
	// after a certain amount of time has passed, the arena will start partially full
	chrono::time_point<chrono::system_clock> start, current;
	// sets the starting point of the timer
	start = chrono::system_clock::now();
	// the total elapsed time since the timer started
	chrono::duration<double> elapsed_seconds;
	
	// the amount of time (seconds) to wait after creation before the arena starts partially filled
	double waiting_limit = 40.0;
	
	// runs until the game is ready to start
	while (!ready_to_start) {
		// check if the arena has filled up
		if (num_players >= MAX_PLAYERS) {
			ready_to_start = true;
		}
		
		// get the current time and find the total elapsed time
		current = chrono::system_clock::now();
		elapsed_seconds = current - start;
		// check if the timer has exceeded the waiting limit
		if (elapsed_seconds.count() > waiting_limit) {
			ready_to_start = true;
		}
	}
	
	// signal that the arena is no longer accepting players, in case it wasn't already set
	accepting_players = false;
	
	// get everything set up
	setup();
	
	// the game is ready to start, call the game loop
	game_loop();
	
	// free used memory and be prepared to restart the arena
	clean_up();
}

// handles everything that needs to happen before the game can start
void Arena::setup() {
	// give every player a starting position
	init_player_positions();
	
	// create the walls
	wall_manager.start();
	
	// set up the bomb manager and its timer
	bomb_manager.start();
	
	// send out the first message to show the starting positions
	send_message();
}


// the main loop for the game, handles everything that relates to the game
void Arena::game_loop() {
	// exit condition
	bool end_game = false;
	
	// regulate frames per second by waiting at the end of the loop until enough time has passed
	// create a time point for the starting time of each frame and one to check if enough time has passed
	chrono::time_point<chrono::system_clock> start, current;
	// sets the starting point of the timer
	start = chrono::system_clock::now();
	// the time duration to be used as the stopwatch for each frame
	chrono::duration<double> elapsed_seconds;
	
	// desired time (in milliseconds) of each frame
	int frame_time_ms = 25;
	// desired time (in seconds) of each frame
	double frame_time = (double) frame_time_ms / 1000;
	
	
	// the game loop
	while (!end_game) {
		// iterate through each message and update the player's velocity
		process_messages();
		
		/*
		handle all game related activity
		*/
		
		// move each player according to its velocity
		update_player_positions();
		// move each projectile according to its velocity
		update_projectiles();
		// update the walls
		update_walls();
		// update the bombs, everything can be done by the bomb manager
		bomb_manager.update_bombs();
		
		// compile all relevant data into a string message and push it to the outgoing queue
		send_message();
		
		
		/*
		regulate the frame rate
		*/
		
		// get the current time and find how much time has passed since the start of the frame
		current = chrono::system_clock::now();
		elapsed_seconds = start - current;
		
		// continuously update the elapsed time until it reaches the target time
		while (elapsed_seconds.count() < frame_time) {
			current = chrono::system_clock::now();
			elapsed_seconds = current - start;
		}
		
		// update the start time to when the next frame should start
		// will always slightly overshoot the target time, this compensates for that
		start += chrono::milliseconds(frame_time_ms);
		
		// check for a winner
		if (arena_players.size() <= 1) {
			end_game = true;
		}
	}
	
}

// gives every player a starting position from a preset list of positions
void Arena::init_player_positions() {
	// set the seed for the random number generator
	srand(time(NULL));
	int count = 0;
	
	vector<point> starting_positions;
	// upper left
	starting_positions.push_back(point(140, 160));
	// lower right
	starting_positions.push_back(point(820, 480));
	// lower left
	starting_positions.push_back(point(140, 480));
	// upper right
	starting_positions.push_back(point(820, 160));
	
	for (Player* player : arena_players) {
		player->posX = starting_positions[count].x;
		player->posY = starting_positions[count].y;
		count++;
		continue;
	}
}

// go through the queue of messages and take the actions associated with each message
void Arena::process_messages() {
	lock_guard<mutex> guard(arena_lock);
	// process messages until there are no more in the queue
	while (!incoming_queue.empty()) {
		// pop the top message off tthe queue
		message_struct* msg = incoming_queue.front();
		incoming_queue.pop();
		
		// the container to be used for the pieces of data in the message
		vector<string> data;
		// split the message at each comma to obtain individual values to be used
		boost::split(data, msg->message_text, boost::is_any_of(","));
		
		// set the velocity for the player according to the message data
		// set the shooting status for the player
		try {
			msg->player->rotationVel = stoi(data[0]);
			msg->player->vel = stoi(data[1]);
			
			bool space_pressed;
			if (stoi(data[2]) == 1) {
				space_pressed = true;
			} else {
				space_pressed = false;
			}
			if (space_pressed && msg->player->ready_to_shoot) {
				msg->player->shoot_projectile = true;
				msg->player->ready_to_shoot = false;
			} else if (space_pressed) {
				msg->player->shoot_projectile = false;
			} else if (!space_pressed) {
				msg->player->shoot_projectile = false;
				msg->player->ready_to_shoot = true;
			}
		} catch (exception e) {
			// catch errors but do nothing about them
		}
		
		// delete the message struct and free the memory
		msg->player = NULL;
		delete msg;
	}
}

// update the position of each player according to its velocity, as long as the move is valid
void Arena::update_player_positions() {
	for (Player* player : arena_players) {
		// separate the movement into 5 movements by a single pixel, check for a collision each time
		int i = 0;
		bool collision = false;
		bool delete_player = false;
		
		while ((i < MOVEMENT_PER_FRAME) && (!collision)) {
			/*
			if there will be no collision with a movement, move the player
			*/

			player->newRotation = player->rotation + player->rotationVel;
			player->newRotation = (player->newRotation + 360) % 360;
			
			player->velX = - player->vel * sin(player->newRotation * (M_PI / 180));
			player->velY = player->vel * cos(player->newRotation * (M_PI / 180));
			
			player->newX = player->posX + player->velX;
			player->newY = player->posY + player->velY;
			
			/*
			check for wall collisions
			- update rectangle points
			- for each point, check if it overlaps with wall
			*/
			
			// check if each point is outside the boundary
			player->update_rectangle_points();
			for (point p : player->body.points) {
				if ((p.x <= 0) || (p.x >= SCREEN_WIDTH) || (p.y <= 0) || (p.y >= SCREEN_HEIGHT)) {
					collision = true;
					break;
				}
			}
			
			// break now for efficiency
			if (collision) {
				break;
			}
			
			for (Player* other_player : arena_players) {
				// if both players are the same, skip to next iteration
				if (player->color == other_player->color) {
					continue;
				}
				
				other_player->update_rectangle_points();
				
				bool c = Collisions::polygon_collision(player->body, other_player->body);
				if (c) {
					collision = true;
				}
			}
			
			// break now for efficiency
			if (collision) {
				break;
			}
			
			for (Wall* wall : wall_manager.walls) {
				wall->newRotation = wall->rotation;
				wall->update_points();
				bool c = Collisions::polygon_collision(player->body, wall->body);
				if (c) {
					collision = true;
				}
			}
			
			// break now for efficiency
			if (collision) {
				break;
			}
			
			for (Bomb* bomb : bomb_manager.bombs) {
				bool c = Collisions::bomb_player_collision(bomb, player->body);
				if (c) {
					delete_player = true;
					collision = true;
				}
			}
			
			if (!collision) {
				// no collision, move the position and rotation in the direction of the velocity
				player->posX = player->newX;
				player->posY = player->newY;
				player->rotation = player->newRotation;
			}
			i++;
		}
		
		if (delete_player) {
			arena_players.erase(player);
			dead_players.insert(player);
			continue;
		}
		
		// if the player has shot a projectile, create the projectile and add it to the set
		if (player->shoot_projectile) {
			Projectile* projectile = new Projectile(player->posX, player->posY,
													player->rotation, Player::PLAYER_HEIGHT);
			projectile->shooter_color = player->color;
			projectiles.insert(projectile);
			player->shoot_projectile = false;
		}
	}
}

// update the positions and check collisions for each projectile
void Arena::update_projectiles() {
	for (Projectile* projectile : projectiles) {
		
		int i = 0;
		bool exit = false;
		bool was_deleted = false;
		
		while ((i < projectile->PROJECTILE_SPEED) && (!exit)) {
			
			projectile->posX += projectile->velX;
			projectile->posY += projectile->velY;
			
			/*
			check boundary collision
			*/
			
			if (projectile->posX <= Projectile::RADIUS) {
				projectile->velX = abs(projectile->velX);
			}
			if (projectile->posX >= (SCREEN_WIDTH - Projectile::RADIUS)) {
				projectile->velX = - abs(projectile->velX);
			}
			if (projectile->posY <= Projectile::RADIUS) {
				projectile->velY = abs(projectile->velY);
			}
			if (projectile->posY >= (SCREEN_HEIGHT - Projectile::RADIUS)) {
				projectile->velY = - abs(projectile->velY);
			}
			
			/*
			checks wall collision
			*/
			
			for (Wall* wall : wall_manager.walls) {
				wall->newRotation = wall->rotation;
				wall->update_points();
				// signals that the ball should be deflected on collision
				bool deflect = true;
				int c = Collisions::wall_ball_collision(projectile, wall, deflect);
				if (c == 2) {
					projectiles.erase(projectile);
					delete projectile;
					return;
				}
			}
			
			/*
			check player collision
			*/
			
			for (Player* player : arena_players) {
				// get player ready to find actual rectangle corners
				player->reset_temp_vars();
				player->update_rectangle_points();
				// check collision between the player and the ball
				bool collision = Collisions::ball_player_collision(projectile, player->body);
				if (collision) {
					// checks to make sure the player isn't killed by the projectile it just fired
					if ((projectile->tick_count > 2) || (projectile->shooter_color != player->color)) {
						arena_players.erase(player);
						dead_players.insert(player);
						projectiles.erase(projectile);
						delete projectile;
						was_deleted = true;
						exit = true;
						break;
					}
				}
			}
			
			i++;
		}
		
		// if the ball was not deleted after a collision, increment its tick count
		if (!was_deleted) {
			projectile->tick_count++;
			projectile->ticks_since_deflection++;
		}
		
	}
}

// check if a wall can rotate and update each wall
void Arena::update_walls() {
	// check each rotating wall for the ability to rotate another step
	for (Wall* wall : wall_manager.rotating_walls) {
		wall->newRotation = wall->rotation + wall->rotationVel;
		wall->update_points();
		wall->can_rotate = true;
		
		// check for a collision with a player
		for (Player* player : arena_players) {
			bool player_collision = Collisions::polygon_collision(player->body, wall->body);
			if (player_collision) {
				wall->can_rotate = false;
			}
		}
		
		// check for a collision with a ball
		for (Projectile* projectile : projectiles) {
			// signals that the ball should not be deflected, only checking for the wall's rotation
			bool deflect = false;
			int ball_collision = Collisions::wall_ball_collision(projectile, wall, deflect);
			if (ball_collision == 1) {
				wall->can_rotate = false;
			}
		}
	}
	
	wall_manager.update_walls();
}

// sends out a message with the color and coordinates of each player to draw
/*
example output message (player section only):
	"blue,100,100,0,green,300,300,90,red,500,500,180"
*/
void Arena::send_message() {
	string message = "";
	
	// used for omitting the first comma
	int i = 0;
	
	// add each player's data to the message
	for (Player* player : arena_players) {
		// don't add a comma if it is the first element of the message
		if (i != 0) {
			message += ",";
		}
		
		// append the message string with the data
		message += player->color;
		message += "," + to_string((int) (player->posX + 0.5));
		message += "," + to_string((int) (player->posY + 0.5));
		message += "," + to_string(player->rotation);
		
		i++;
	}
	
	message += "/";
	i = 0;
	
	// add each wall's data to the message
	for (Wall* wall : wall_manager.walls) {
		if (i != 0) {
			message += ",";
		}
		
		message += to_string(wall->posX);
		message += "," + to_string(wall->posY);
		message += "," + to_string(wall->rotation);
		
		i++;
	}
	
	message += "/";
	i = 0;
	
	// necessary since there may not be any bombs but there must be something between /'s
	message += "=====,";
	
	// add each projectile's data to the message
	for (Bomb* bomb : bomb_manager.bombs) {
		if (i != 0) {
			message += ",";
		}
		
		message += to_string(bomb->posX);
		message += "," + to_string(bomb->posY);
		message += "," + to_string((int) bomb->radius);
		message += "," + to_string((int) bomb->warning_mode);
		
		i++;
	}
	
	message += "/";
	i = 0;
	
	// add each projectile's data to the message
	for (Projectile* projectile : projectiles) {
		if (i != 0) {
			message += ",";
		}
		
		message += to_string((int) (projectile->posX + 0.5));
		message += "," + to_string((int) (projectile->posY + 0.5));
		
		i++;
	}
	
	// send the message to the queue to be sent to players
	add_to_outgoing_queue(message);
}

// locks the arena lock, called by the server
void Arena::lock_mutex() {
	arena_lock.lock();
}

// releases the arena lock, called by the server
void Arena::unlock_mutex() {
	arena_lock.unlock();
}

// cleans the arena after a game has ended
void Arena::clean_up() {
	// ensure that nothing is messed up by being accessed during the clean up
	lock_mutex();
	
	// clear the list of players, both alive and dead
	// player objects are deleted in gameserver after this function has finished
	arena_players.clear();
	dead_players.clear();
	
	// clear the message queue and deallocate the memory for each message
	while (!incoming_queue.empty()) {
		message_struct* msg = incoming_queue.front();
		incoming_queue.pop();
		msg->player = NULL;
		delete msg;
	}
	
	// nothing to deallocate here, just need to clear the queue
	while (!outgoing_queue.empty()) {
		outgoing_queue.pop();
	}
	
	// remove each projectile from the set and delete it
	for (Projectile* projectile : projectiles) {
		projectiles.erase(projectile);
		delete projectile;
	}
	
	// delete the walls, handled by the wall manager
	wall_manager.clean_up();
	
	// delete the bombs, handled by the bomb manager
	bomb_manager.clean_up();
	
	// signal to gameserver that the arena is done being cleaned
	ready_to_reset = true;
	ready_to_start = false;
	num_players = 0;
	color_index = 0;
	accepting_players = true;
	
	// release the arena lock
	unlock_mutex();
	
}



