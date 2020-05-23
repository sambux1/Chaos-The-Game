/*
gameserver class

Chaos The Game
server v0.2

This server will accept WebSocket connections from multiple clients and allow
the clients to communicate with each other by displaying messages sent from any
individual one.

Listening on server's localhost address. A proxy server redirects websocket connections and messages
to port 8080, where this program is listening.
*/


// this project uses the websocketpp library to handle WebSocket communication
#include <websocketpp/server.hpp>
// uses insecure websockets, proxy handles security with SSL
#include <websocketpp/config/asio_no_tls.hpp>

// include the header file
#include "gameserver.h"

// include other server files
#include "arena.h"
#include "player.h"
#include "message_struct.h"

// include other dependencies
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <chrono>
#include <mutex>

using namespace std;

// connection handler
using websocketpp::connection_hdl;
// used as argument for callback functions
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
// used to assign callback functions to events
using websocketpp::lib::bind;


// handled all activity associated with an arena thread
void run_arena_thread(Arena* arena);


// constructor, initializes server and sets callback functions
gameserver::gameserver() {
	// initialize boost::asio connection functionality
	m_server.init_asio();
	
	// eliminates the 30ish second cooldown period before the port opens again
	m_server.set_reuse_addr(true);
	
	// callback functions
	// ::_1 and ::_2 are placeholders to indicate that more arguments will be passed
	m_server.set_open_handler(bind(&gameserver::on_open, this, ::_1));
	m_server.set_close_handler(bind(&gameserver::on_close, this, ::_1));
	m_server.set_message_handler(bind(&gameserver::on_message, this, ::_1, ::_2));
}

// destructor, does not do anything yet
gameserver::~gameserver() {

}

// callback function for when a new connection is created
// signals for a new player to be created and added to an arena
void gameserver::on_open(connection_hdl handler) {
	// locks the action queue so an action can be pushed
	websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
	m_actions.push(action(CONNECT, handler));
}

// callback function for when a connection is closed
void gameserver::on_close(connection_hdl handler) {
	// locks the action queue so an action can be pushed
	websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
	m_actions.push(action(DISCONNECT, handler));
}

// callback function for when a message is received by the server
// creates a message struct and adds the message to the message queue of the appropriate arena
void gameserver::on_message(connection_hdl handler, server::message_ptr message) {
	// locks the action queue so an action can be pushed
	websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_action_lock);
	m_actions.push(action(MESSAGE, handler, message));
}

// starts the server
// the port is a 16 bit integer (0 to 65535)
void gameserver::run(uint16_t port) {
	// arena should be created before listening begins, in case of an immediate connection
	// create the arena to be run in the thread
	arena = new Arena();
	// start a thread to run the arena in parallel with the event loop
	thread arena_thread(run_arena_thread, arena);
	
	// begin listening for connections on the port given
	m_server.listen(port);
	// begin accepting connections
	m_server.start_accept();
	// begin the main event loop
	m_server.run();
	
	// join the thread, thread process does not stop until the main event loop is terminated
	arena_thread.join();
}

/*
the main event loop of the server
runs in a separate thread from the event listeners
sends out all messages in the outgoing queue, then processes actions from the action queue
*/
void gameserver::process_actions() {
	while (true) {
		
		// process thread is started slightly before arena is created
		if (arena != NULL) {
			send_messages();
		}
		
		// if there are no actions to be taken, release the lock and continue to the next iteration
		// cannot wait for the condition to be notified because then outgoing messages
		// 		would be delayed until a new message is received
		if (m_actions.empty()) {
			continue;
		}
		
		// locks the action queue
		websocketpp::lib::unique_lock<websocketpp::lib::mutex> lock(m_action_lock);
		
		// pop the top action off the queue
		action a = m_actions.front();
		m_actions.pop();
		
		// release the lock to the action queue
		lock.unlock();
		
		// handle each type of action
		if (a.type == CONNECT) {
			// adds a new player
			add_connection(a.handler);
		} else if (a.type == DISCONNECT) {
			// removes a player
			remove_connection(a.handler);
		} else if (a.type == MESSAGE) {
			// route an incoming message to the appropriate arena
			process_incoming_message(a.handler, a.message);
		}
	}
}

// adds the new connection to the list of connections, creates a player, and assigns the player to the arena
void gameserver::add_connection(connection_hdl handler) {
	// locks the connections list so one can be added
	{
		websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);
		// add the connection handler to the set of connections
		m_connections.insert(handler);
	}
	
	// create a new player identifier
	player_id* new_player = new player_id;
	
	// add a new player object to the identifier
	new_player->player = new Player();
	
	// add the connection handler and the player identifier to the map of players
	m_player_map.insert(pair<connection_hdl, player_id*>(handler, new_player));
	
	// assign the player to an arena, fills the parent_arena field in the playeridentifier
	assign_to_arena(new_player);
}

// removes the connection from the list of connections
// will need to do more once there are multiple arenas
void gameserver::remove_connection(connection_hdl handler) {
	// locks the connections list so one can be removed
	{
		websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);
		// remove a connection from the list
		m_connections.erase(handler);
	}
}

// receives a new message and sends the message to the arena
void gameserver::process_incoming_message(connection_hdl handler, server::message_ptr message) {
	// creates a new message struct
	message_struct* msg = new message_struct;
	
	// connect the message with the player that sent it, based on player's connection handler
	msg->player = m_player_map[handler]->player;
	
	// set the message text to the body of the received message
	msg->message_text = message->get_payload();
	
	// add the message to the arena's message queue
	// when there are multiple arenas, it will need to find the correct one
	// the arena handles locking
	arena->add_to_incoming_queue(msg);
}

/*
sends a message from the arena to all members of the arena
since there is only one arena for now, messages can be sent to all connections
once there are more arenas, will need to find the appropriate arena and its members
*/
// ********TODO: make this actually send dummy messages, then real messages
void gameserver::send_messages() {
	// lock the arena outgoing queue
	arena->lock_mutex();
	
	// send all messages in the queue
	while (!arena->outgoing_queue.empty()) {
		// pop the top message off the queue
		string message = arena->outgoing_queue.front();
		arena->outgoing_queue.pop();
		
		// send message to all connections (to be modified for multiple arenas)
		for (connection_hdl handler : m_connections) {
			// sends the message to the player, sets opcode to 1 for text data
			m_server.send(handler, message, websocketpp::frame::opcode::value(1));
		}
	}
	
	// release the arena lock
	arena->unlock_mutex();
}


// adds a new player to the arena, locking is handled by the arena
void gameserver::assign_to_arena(player_id* new_player) {
	// assigns the arena to the player identifier
	new_player->parent_arena = arena;
	
	// adds the player to the arena
	// does not check for error, will need to with multiple arenas
	arena->add_player(new_player->player);
}


// function to run the arena thread
void run_arena_thread(Arena* arena) {
	// calls the arenas start loop, will begin game loop when ready
	arena->start();
}


int main() {
	// create the game server
	gameserver gs;
	// create a new thread to perform actions loaded onto the action queue
	websocketpp::lib::thread action_thread(bind(&gameserver::process_actions, &gs));
	// run the main event loop on the server to listen for events
	gs.run(8080);
	// end the action processing thread when the server stops running
	action_thread.join();
}

