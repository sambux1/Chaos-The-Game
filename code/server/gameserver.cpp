/*
gameserver class

Chaos The Game
server v0.1

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

#include <iostream>
#include <string>
#include <set>

using namespace std;

using websocketpp::connection_hdl;		// connection handler
using websocketpp::lib::placeholders::_1;	// used as argument for callback functions
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;			// used to assign callback functions to events



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

// callback function for when a new connection is created
void gameserver::on_open(connection_hdl handler) {
	m_connections.insert(handler);
}

// callback function for when a connection is closed
void gameserver::on_close(connection_hdl handler) {
	m_connections.erase(handler);
}

// callback function for when a message is sent to the server
void gameserver::on_message(connection_hdl handler, server::message_ptr message) {
	for (connection_hdl x : m_connections) {
		m_server.send(x, message);
	}
}

// starts the server
// the port is a 16 bit integer (0 to 65535)
void gameserver::run(uint16_t port) {
	// begin listening for connections on the port given
	m_server.listen(port);
	// begin accepting connections
	m_server.start_accept();
	// begin the main event loop
	m_server.run();
}


int main() {
	gameserver gs;
	gs.run(8080);
}

