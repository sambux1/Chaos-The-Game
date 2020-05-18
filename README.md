#Chaos The Game

www.chaos-the-game.com

A fast-paced online multiplayer browser game where players battle in an arena through various obstacles.

The client-side uses JavaScript and the HTML5 Canvas element for graphics. They communicate with the server via websocket connections.

The server uses the C++ websocketpp library and runs on a DigitalOcean droplet. Nginx proxies all websocket communication to the localhost port 8080, where the server program is listening. Communication is secure with an SSL key from LetsEncrypt. Incoming messages use secure websocket connections, and the proxy handles the security and sends them to the server program as insecure websockets.


##v0.1
The website is currently an online chatroom. When a new user joins, they can begin sending messages, and all users connected will see the messages appear on the screen. This is the initial test of online functionality.

In this version, I set up a DigitalOcean droplet to run the server, created prototypes for the client and server programs, and configured Nginx to proxy all websocket communication to the server program.

##Next Steps - v0.2
I will be creating a dummy arena where multiple players can join and move around as blocks. Their movement will be communicated with the server and reflected in all clients. It will handle collisions between players and with the boundaries.

There will be no lag compensation yet. All movement will be sent to the server and processed before showing anything to the screen. I will be testing different framerates as well to help determine how lag compensation should work.
