# Chaos The Game

www.chaos-the-game.com

A fast-paced online multiplayer browser game where players battle in an arena through various obstacles.

The client-side uses JavaScript and the HTML5 Canvas element for graphics. They communicate with the server via websocket connections.

The server uses the C++ websocketpp library and runs on a DigitalOcean droplet. Nginx proxies all websocket communication to the localhost port 8080, where the server program is listening. Communication is secure with an SSL key from LetsEncrypt. Incoming messages use secure websocket connections, and the proxy handles the security and sends them to the server program as insecure websockets.

## v0.3 - Basic Arena
- new movement mechanic that allows for rotation
- player sprites
- players can shoot projectiles that can eliminate other players
- much better collision detection between players, within one pixel of accuracy for rotated rectangles
- collision detection for walls, players, and projectiles
- the core of the game has been finished, and most of what is left involves adding features to the game

## v0.2 - Dummy Arena
- players can move blocks around the screen using the arrow keys
- the client sends key presses to the server, and the server processes the data and sends back all information needed to draw to the screen
- collisions are not visually ideal, but this is not the final movement mechanic
- the server is designed to handle up to 4 players, though nothing is stopping it from serving many more
- the server runs 3 threads: one to receive and send messages, one to transfer messages to the arena, and one to run the game in the arena
- an additional thread will be needed for each additional arena
- the majority of the server setup is finished now, and most of the work remaining will be creating the actual game

## v0.1 - Online Test
- initial test of online functionality
- an online chatroom where a user can send a message to all other connected users
- set up a DigitalOcean droplet to run the server
- configured Nginx to proxy all communication to a websocket listener
