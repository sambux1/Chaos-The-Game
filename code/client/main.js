/*
Client v0.1

A connection is made to the server via websockets, and messages are sent and received through 
this connection.
*/

var websocket;

function connect() {
	
	// create a new connection to the server, uses secure websockets
	websocket = new WebSocket("wss://chaos-the-game.com/websocket:443");
	
	/*
	this is used for testing on my computer
	websocket = new WebSocket('ws://localhost:8080');
	*/
	
	// callback function for when a connection is first established
	websocket.onopen = function() {
		var name = document.getElementById("nameTextBox").value;
		var message = "*** " + name + " joined the chat";
		websocket.send(message);
	};
	
	// callback function for new messages
	websocket.onmessage = function(event) {
		var message = event.data;
		// split the data into individual components using the commas
		var data = message.split(',');
		var index = 0;
		
		var canvas = document.getElementById("canvas");
		var context = canvas.getContext("2d");
		
		context.clearRect(0, 0, canvas.width, canvas.height);
		
		while (index < data.length) {
			context.fillStyle = data[index];
			context.fillRect(parseInt(data[index+1]), parseInt(data[index+2]), 150, 100);
			index += 3;
		}
	};
	
}

// listens for a key press and send it to the server
document.addEventListener('keydown', sendData);

function sendData(event) {
	var message;
	if (event.key == 'ArrowUp') {
		message = '0,-1';
	} else if (event.key == 'ArrowDown') {
		message = '0,1';
	} else if (event.key == 'ArrowLeft') {
		message = '-1,0';
	} else if (event.key == 'ArrowRight') {
		message = '1,0';
	}
	
	websocket.send(message);
}



