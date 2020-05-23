# Chaos The Game

www.chaos-the-game.com

A fast-paced online multiplayer browser game where players battle in an arena through various obstacles.

The client-side uses JavaScript and the HTML5 Canvas element for graphics. They communicate with the server via websocket connections.

The server uses the C++ websocketpp library and runs on a DigitalOcean droplet. Nginx proxies all websocket communication to the localhost port 8080, where the server program is listening. Communication is secure with an SSL key from LetsEncrypt. Incoming messages use secure websocket connections, and the proxy handles the security and sends them to the server program as insecure websockets.

## v0.2
The website is now a dummy arena where up to 4 players can join and move blocks around the screen (though the server can handle more than 4 players). The client sends key presses to the server and the server processes the data and sends back all the information needed to draw the new screen. The collisions are not visually ideal, but this is not the actual movement mechanism, just a dummy prototype.

The server now runs 3 threads: one to receive incoming messages, one to process those messages and send them to the arena, and one to run the arena. New arenas will each require an additional thread.

The majority of the server setup and communication is finished now, and most of the work remaining will be creating the actual game.

## v0.1
The website is currently an online chatroom. When a new user joins, they can begin sending messages, and all users connected will see the messages appear on the screen. This is the initial test of online functionality.

In this version, I set up a DigitalOcean droplet to run the server, created prototypes for the client and server programs, and configured Nginx to proxy all websocket communication to the server program.
