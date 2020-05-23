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

// include other dependencies
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
// used for random number generation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// include boost string splitter
#include <boost/algorithm/string.hpp>

using namespace std;

// the colors that will be assigned to the players
const string Arena::color_list[] = {"blue", "green", "red", "orange"};


// constructor
Arena::Arena() {
	num_players = 0;
	accepting_players = true;
	ready_to_start = false;
	color_index = 0;
}

// destructor, does not do anything yet
Arena::~Arena() {
	
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
	
	// check if the arena should be closed off to new players
	if (num_players >= MAX_PLAYERS) {
		accepting_players = false;
	}
	
	// assign the player the next color in the list and increment the index
	player->color = color_list[color_index];
	color_index++;
	
	// if the function reaches this point, the addition was successful
	return true;
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
	// sets a timer for how long the arena can be waiting for players without starting
	// after a certain amount of time has passed, the arena will start partially full
	chrono::time_point<chrono::system_clock> start, current;
	// sets the starting point of the timer
	start = chrono::system_clock::now();
	// the total elapsed time since the timer started
	chrono::duration<double> elapsed_seconds;
	
	// the amount of time (seconds) to wait after creation before the arena starts partially filled
	double waiting_limit = 20.0;
	
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
}

// handles everything that needs to happen before the game can start
void Arena::setup() {
	// give every player a random starting position, check to make sure they are valid
	init_player_positions();
	
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
		
		// move each player according to its velocity
		update_player_positions();
		
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
	}
}

// gives every player a random starting position and ensures that the position is valid
void Arena::init_player_positions() {
	// set the seed for the random number generator
	srand(time(NULL));
	
	for (Player* player : arena_players) {
		// used for looping until valid coordinates are found
		bool valid = false;
		
		// coordinates
		int xStart, yStart;
		
		while (!valid) {
			// set valid to true, if no collisions are found, the loop will end
			valid = true;
			
			// find random starting coordinates
			xStart = rand() % (SCREEN_WIDTH - Player::PLAYER_WIDTH);
			yStart = rand() % (SCREEN_HEIGHT - Player::PLAYER_HEIGHT);
			
			// check for collisions with the other starting coordinates
			for (Player* other_player : arena_players) {
				// if the other player's coordinates have not been set, continue to next player
				if (other_player->posX == -1) {
					continue;
				}
				
				// find the distance between the players along both axes
				int distanceX = abs(xStart - other_player->posX);
				int distanceY = abs(yStart - other_player->posY);
				
				// check for collision
				if ((distanceX <= Player::PLAYER_WIDTH) && (distanceY <= Player::PLAYER_HEIGHT)) {
					// if there is a collision, set valid to false so the loop will repeat, then break
					valid = false;
					break;
				}
			}
			
			// once this has been reached, valid will be true if no collisions were found
			
		}
		
		// coordinates have been found, assign them to the player
		player->posX = xStart;
		player->posY = yStart;
	}
}

// go through the queue of messages and take the actions associated with each message
void Arena::process_messages() {
	arena_lock.lock();
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
		msg->player->velX = stoi(data[0]);
		msg->player->velY = stoi(data[1]);
	}
	
	arena_lock.unlock();
}

// update the position of each player according to its velocity, as long as the move is valid
void Arena::update_player_positions() {
	for (Player* player : arena_players) {
		/*
		find how far away the player is from each other player
		if there will be no collision with a "full" movement (5 pixels), move the player the full amount
		if there will be a collision, move the player to occupy as much room as it can without colliding
		*/
		
		int newX = player->posX + (5 * player->velX);
		int newY = player->posY + (5 * player->velY);
		
		// check for collision with the wall
		if ((newX < 0) || ((newX + 150) > SCREEN_WIDTH) || (newY < 0) || ((newY + 100) > SCREEN_HEIGHT)) {
			continue;
		}
		
		bool collision = false;
		
		for (Player* other_player : arena_players) {
			// if both players are the same, skip to next iteration
			if (player->color == other_player->color) {
				continue;
			}
			
			// check for collision if movement is allowed
			int distanceX = newX - other_player->posX;
			distanceX = abs(distanceX) - 150;
			int distanceY = newY - other_player->posY;
			distanceY = abs(distanceY) - 100;
			
			if ((distanceX < 5) && (distanceY < 5)) {
				collision = true;
			}
		}
		
		if (!collision) {
			// move the position in the direction of the velocity, multiplied by a scaling factor
			player->posX = newX;
			player->posY = newY;
		}
	}
}

// sends out a message with the color and coordinateds of each player to draw
// will expand the data sent as the game develops
/*
example output message:
	"blue,100,100,green,300,300,red,500,500"
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
		message += "," + to_string(player->posX);
		message += "," + to_string(player->posY);
		
		i++;
	}
	
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


