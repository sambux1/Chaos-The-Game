/*
gameserver class

Chaos The Game

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
	// signals that arenas should not be acted upon until they have been fully set up
	arenas_ready = false;
	
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

// destructor
gameserver::~gameserver() {
	// empty all data structures
	
	while (!m_actions.empty()) {
		m_actions.pop();
	}
	
	while (arenas.size() > 0) {
		Arena* arena = arenas[0];
		arenas.erase(arenas.begin());
		delete arena;
	}
	
	for (connection_hdl handler : m_connections) {
		remove_connection(handler);
	}
	
	arena_players_map.clear();
	
	m_player_map.clear();
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
	for (int i = 0; i < num_arenas; i++) {
		Arena* arena = new Arena();
		connection_list arena_connections;
		arena_players_map.insert(pair<Arena*, connection_list>(arena, arena_connections));
		arenas.push_back(arena);
	}
	
	arenas_ready = true;
	
	// create a new thread for each arena
	thread a0(run_arena_thread, arenas[0]);
	thread a1(run_arena_thread, arenas[1]);
	thread a2(run_arena_thread, arenas[2]);
	
	// begin listening for connections on the port given
	m_server.listen(port);
	// begin accepting connections
	m_server.start_accept();
	// begin the main event loop
	m_server.run();
	
	a2.join();
	a1.join();
	a0.join();
	
}

/*
the main event loop of the server
runs in a separate thread from the event listeners
sends out all messages in the outgoing queue, then processes actions from the action queue
*/
void gameserver::process_actions() {
	while (true) {
		
		// each iteration, send all messages before processing any action
		send_messages();
		
		// if there are no actions to be taken, continue to the next iteration
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
	assign_to_arena(new_player, handler);
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
	
	// deletes the player
	player_id* player_id = m_player_map[handler];
	player_id->parent_arena->remove_player(player_id->player);
	player_id->parent_arena = NULL;
	delete player_id->player;
	delete player_id;
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
	// the arena handles locking
	m_player_map[handler]->parent_arena->add_to_incoming_queue(msg);
}


// sends a message from the arena to all members of the arena
void gameserver::send_messages() {
	// ensures the arenas are not accessed until they have been properly set up
	if (!arenas_ready) {
		return;
	}
	
	for (Arena* arena : arenas) {
		// check if the arena's game has finished and needs to be reset
		// exists here because the arena's connection list cannot be accessed from static method
		if (arena->ready_to_reset) {
			reset_arena(arena);
			continue;
		}
		
		// lock the arena outgoing queue
		arena->lock_mutex();
		
		// send all messages in the queue
		while (!arena->outgoing_queue.empty()) {
			// pop the top message off the queue
			string message = arena->outgoing_queue.front();
			arena->outgoing_queue.pop();
			
			// send message to all connections in the arena
			for (connection_hdl handler : arena_players_map[arena]) {
				try {
					// sends the message to the player, sets opcode to 1 for text data
					m_server.send(handler, message, websocketpp::frame::opcode::value(1));
				} catch (exception e) {
					// error sending message
				}
			}
		}
		
		// release the arena lock
		arena->unlock_mutex();
	}
}


// adds a new player to the arena, locking is handled by the arena
void gameserver::assign_to_arena(player_id* new_player, connection_hdl handler) {
	// check for first open arena
	for (Arena* arena : arenas) {
		if (arena->add_player(new_player->player)) {
			// assigns the arena to the player identifier
			new_player->parent_arena = arena;

			// locks the connections list so one can be added
			{
				websocketpp::lib::lock_guard<websocketpp::lib::mutex> guard(m_connection_lock);
				// add the connection handler to the set of connections
				arena_players_map[arena].insert(handler);
			}
			
			// added to an arena, do not add to multiple arenas
			break;
		}
	}
}

// prepares the arena for a new game
// clears the connection list for the arena
void gameserver::reset_arena(Arena* arena) {
	// removes each connection from the arena
	arena_players_map[arena].clear();
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

