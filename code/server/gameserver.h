/*
gameserver class header file

Chaos The Game
server v0.1

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

#include <set>

using namespace std;

using websocketpp::connection_hdl;	// connection handler

// defines the server type used by websocketpp
typedef websocketpp::server<websocketpp::config::asio> server;
// defines the type of the set of connections
typedef set<connection_hdl, owner_less<connection_hdl>> connection_list;


class gameserver {

public:
	gameserver();
	void on_open(connection_hdl handler);
	void on_close(connection_hdl handler);
	void on_message(connection_hdl handler, server::message_ptr message);
	void run(uint16_t port);

private:
	server m_server;
	connection_list m_connections;

};

#endif
