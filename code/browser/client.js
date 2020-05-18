// connect to localhost WebSocket server
// TEST
//<canvas id="canvas" width="900" height="600" style="border:1px solid #000000;"></canvas>

var websocket;

function connect() {
	websocket = new WebSocket("wss://chaos-the-game.com/websocket:443");
	websocket.onopen = function() {
		var name = document.getElementById("nameTextBox").value;
		var message = "*** " + name + " joined the chat";
		websocket.send(message);
	};
	
	websocket.onmessage = function(event) {
		var message = document.createElement("P");
		message.innerText = event.data;
		document.body.appendChild(message);
	};
	
}

function send() {
	var name = document.getElementById("nameTextBox").value;
	var message = document.getElementById("messageTextBox").value;
	var toSend = name + ": " + message;
	websocket.send(toSend);
}
