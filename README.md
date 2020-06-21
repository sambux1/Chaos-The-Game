# Online Multplayer Tank Game

This project was an online game through which I learned about online communication, threading, and setting up a web server.

## Server
The server for this game has been taken down, but once ran at www.chaos-the-game.com.

The server uses the C++ websocketpp library and runs on a DigitalOcean droplet. Nginx proxies all websocket communication to the localhost port 8080, where the server program is listening. Communication is secure with an SSL key from LetsEncrypt. Incoming messages use secure websocket connections, and the proxy handles the security and sends them to the server program as insecure websockets.

The server receives messages from each client with a list of all keys pressed and processes the input, then updates the game state and sends a message to each client with information about how to display the current game state. The server program's communication is primarily contained within the gameserver class.

The client-side uses JavaScript and the HTML5 Canvas element for graphics.

## Threading
The game server uses multiple threads to perform each task of the server in parallel. The communication system runs in two threads: one receives incoming messages and adds them to a queue, and the other processes incoming messages, adds them to an event queue for an individual game instance, and sends messages.

Additionally, each individual game instance runs in its own thread, called an Arena in the code. There are three threads currently configured to run arenas, though that can be easily adjusted. Arenas continuously process messages off their event queues and update the game state, then send the game state to the communication system to be sent to players.

Locks are used to secure information that is accessed across multiple threads, such as the message queue and the event queues.

## Game Overview
The game itself is a simple battle royale tank game. Players rotate and move through the space using the arrow keys and can shoot deadly projectiles using the space bar. The game lasts until only one player remains.

There are also two other main game elements: walls and bombs.

#### Walls
Aside from the walls that mark the exterior boundary of the game space, there are also interior walls which can rotate and be used to deflect projectiles. Interior walls are given set starting and ending configurations, and every so often, one of them begins rotating into its ending configuration. It does so slowly, and only when it will not collide with a player. Projectiles are deflected by the walls and destroyed if they collide with the end of the wall. In their ending configuration, the walls form a structure which is difficult to enter or exit, blocking off the exterior part of the map.

#### Bombs
Bombs are generated every so often and placed in a ring around the outside edge of the map, so players are encouraged to keep toward the middle of the map, especially after the walls have closed. They begin in a warning mode, where they are semi-transparent and cannot harm players. After a few seconds, they detonate and begin to grow in radius before reaching their maximum size, after which they are destroyed. If a player comes in contact with a bomb after they have detonated, the player will be killed.

## Collisions
No collision library was used, and all collision calculations and actions are processed by the collisions class. In this class, there are many methods which deal with different types of collisions, including:
- collisions between polygons using the separating axis theorem
- collisions between circles and rotated rectangles
- collisions between lines and circles
- calculation of deflection angle after colliding with a rotated surface

## Issues
The game has small memory leaks associated with the way the websocketpp library is used, and there are sometimes issues with resetting an arena for a new game after players have quit unexpectedly. However, given that the purpose of the project was to set up a server primarily for practice and for my friends and I to test, these issues are not overly problematic.

