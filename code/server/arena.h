/*
Arena class header file

Chaos The Game

An arena is one game instance. It performs all actions associated with the
single game instance.
*/

#ifndef ARENA_H
#define ARENA_H

// websocketpp library
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

// include game files
#include "player.h"
#include "message_struct.h"
#include "projectile.h"
#include "wall.h"
#include "wall_manager.h"
#include "bomb.h"
#include "bomb_manager.h"

// include other dependencies
#include <queue>
#include <set>
#include <string>
#include <mutex>

using websocketpp::connection_hdl;

using namespace std;

// defines the type of the set of connections
typedef set<connection_hdl, owner_less<connection_hdl>> connection_list;


class Arena {

public:
	Arena();
	~Arena();
	
	// signals to the gameserver to reset the arena and prepare it for a new game
	bool ready_to_reset;
	
	// dimensions of the game screen
	static const int SCREEN_WIDTH = 960;
	static const int SCREEN_HEIGHT = 640;
	
	// adds a player to the arena, returns true if successful
	bool add_player(Player* player);
	// remove a player after they have disconnected
	void remove_player(Player* player);
	
	// game start loop, waits until game is ready to begin, then calls main loop
	void start();
	// runs right before the game loop begins to get everything initialized correctly
	void setup();
	// main game loop
	void game_loop();
	// clean up the arena, free memory, and prepare for a new game
	void clean_up();
	
	// assigns positions to the players before the game starts
	void init_player_positions();
	
	// create walls
	void create_walls();
	
	// go through the messages queue and take the appropriate action for each
	void process_messages();
	
	// update the position of each player according to its velocity
	void update_player_positions();
	
	// update the positions and handle collisions for each projectile
	void update_projectiles();
	
	// check for the ability to rotate a wall and call the wall_manager's update_walls()
	void update_walls();
	
	// adds a message to the outgoing queue with the color and coordinateds of each player to draw
	void send_message();
	
	// add a new message passed from the server to the queue to be processed
	void add_to_incoming_queue(message_struct* new_message);
	
	// add a new message produced by the arena to the queue to be sent
	void add_to_outgoing_queue(string message);
	
	// the list of players in the arena
	set<Player*> arena_players;
	// set to add players to when they die so they are accounted for but not displayed
	set<Player*> dead_players;
		
	// queue of messages received from the server
	queue<message_struct*> incoming_queue;
	// queue of messages for the server to send
	queue<string> outgoing_queue;
	
	// number of players currently in the arena
	int num_players;
	// maximum number of players allowed in an arena
	static const int MAX_PLAYERS = 4;
	
	// functions to lock and unlock the arena lock, called by the server
	void lock_mutex();
	void unlock_mutex();

private:
	// true while the arena is still gathering players and has not exceeded maximum
	// set to false when the max is reached or when the game starts
	bool accepting_players;
	// indicates when the game is ready to begin, start loop will terminate and call game loop
	bool ready_to_start;
	
	// used for locking access to the arena's resources
	mutex arena_lock;
	
	// used for assigning colors to the players
	static const string color_list[];
	// the index of the color to be given to a new player
	int color_index;
	
	// pixels to move and degrees to rotate each frame
	// for players only
	static const int MOVEMENT_PER_FRAME = 5;
	
	// list of projectiles
	set<Projectile*> projectiles;
	
	// handles wall updates
	Wall_Manager wall_manager;
	
	// handles bombs
	Bomb_Manager bomb_manager;

};

#endif
