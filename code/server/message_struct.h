/*
Message struct

A struct to contain information about an individual message

Chaos The Game
*/

#ifndef MESSAGE_STRUCT_H
#define MESSAGE_STRUCT_H


/*
This is a struct to contain an individual message to be passed to the arena.
It contains the player that sent the message and the message string itself.
*/
typedef struct message_struct {
	// a pointer to the player who sent the message
	Player* player;
	// the text of the message
	std::string message_text;
} message_struct;

#endif
