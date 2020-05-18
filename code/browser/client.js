/*
Client v0.1

A connection is made to the server via websockets, and messages are sent and received through 
this connection.
*/

var websocket;

function connect() {
	// create a new connection to the server, uses secure websockets
	websocket = new WebSocket("wss://chaos-the-game.com/websocket:443");
	
	// callback function for when a connection is first established
	websocket.onopen = function() {
		var name = document.getElementById("nameTextBox").value;
		var message = "*** " + name + " joined the chat";
		websocket.send(message);
	};
	
	// callback function for new messages
	websocket.onmessage = function(event) {
		var message = document.createElement("P");
		message.innerText = event.data;
		document.body.appendChild(message);
	};
	
}

// sends a message to the server, contains the name of the sender and the message text
function send() {
	var name = document.getElementById("nameTextBox").value;
	var message = document.getElementById("messageTextBox").value;
	var toSend = name + ": " + message;
	websocket.send(toSend);
}
