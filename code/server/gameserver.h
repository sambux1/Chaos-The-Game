/*
gameserver class header file

Chaos The Game

This server will accept WebSocket connections from multiple clients and allow
the clients to communicate with each other by displaying messages sent from any
individual one.
*/

#ifndef GAMESERVER_H
#define GAMESERVER_H

// this project uses the websocketpp library to handle WebSocket communication
#include <websocketpp/server.hpp>
// uses insecure websockets, proxy handles security with SSL
#include <websocketpp/config/asio_no_tls.hpp>

// include other server files
#include "arena.h"
#include "player.h"
#include "message_struct.h"

// include other dependencies
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <mutex>

using namespace std;

// connection handler
using websocketpp::connection_hdl;

// defines the server type used by websocketpp
typedef websocketpp::server<websocketpp::config::asio> server;
// defines the type of the set of connections
typedef set<connection_hdl, owner_less<connection_hdl>> connection_list;


/*
This is a struct to contain a player and the arena it belongs to.
Used in the map of connection handlers to players, allows for efficient routing of
incoming messages to the corresponding arena (since map find() is log(n) complexity).
*/
typedef struct player_id {
	// a pointer to the player object that is used in the arena
	Player* player;
	// a pointer to the arena that the player belongs to
	Arena* parent_arena;
} player_id;


// the different types of actions the server can perform
enum action_type {
	CONNECT,
	DISCONNECT,
	MESSAGE
};

/*
A struct for an individual action that the server can take
Constructors only initialize values
*/
struct action {
	// constructor for connect and disconnect actions
	action(action_type t, connection_hdl h) : type(t), handler(h) {}
	// constructor for incoming and outgoing messages
	action(action_type t, connection_hdl h, server::message_ptr m)
		: type(t), handler(h), message(m) {}
	
	// the action to be taken
	action_type type;
	// the unique identifier of the player to communicate with
	connection_hdl handler;
	// the message, if it is a message action
	server::message_ptr message;
};


class gameserver {

public:
	gameserver();
	~gameserver();
	
	// callback functions
	void on_open(connection_hdl handler);
	void on_close(connection_hdl handler);
	void on_message(connection_hdl handler, server::message_ptr message);
	
	// runs the main server loop
	void run(uint16_t port);
	
	// assigns a new player to the arena
	void assign_to_arena(player_id* new_player);
	
	// action loop, runs in separate thread from listener event loop
	void process_actions();
	
	// add a player to the list of connections and add to an arena
	void add_connection(connection_hdl handler);
	// remove a player from the list of connections
	void remove_connection(connection_hdl handler);
	// route received messages to the arena
	void process_incoming_message(connection_hdl handler, server::message_ptr message);
	// send messages that have been added to the arena's outgoing message queue
	void send_messages();
	

private:
	// the main websocketpp server object
	server m_server;
	// the list of all currently open connections
	connection_list m_connections;
	// the arena for the game, only one for now, will become a group of arenas later
	Arena* arena;
	// a map of connection identifiers to a struct containing players and their arena
	// allows an incoming message to be efficiently routed to the appropriate arena
	map<connection_hdl, player_id*, owner_less<connection_hdl>> m_player_map;
	
	// action queue, added to by listeners and processed by action loop
	queue<action> m_actions;
	// used for locking access to the action queue
	websocketpp::lib::mutex m_action_lock;
	// used for locking access to the connection list
	websocketpp::lib::mutex m_connection_lock;

};

#endif

