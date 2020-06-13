/*
Client

Chaos The Game

A connection is made to the server via websockets, and messages are sent and received through 
this connection.
*/

var keysDown = [];
var readyToSend = false;
var websocket;

// declaration of player sprites
var blueSprite;
var greenSprite;
var purpleSprite;
var orangeSprite;

function connect() {
	
	// create a new connection to the server, uses secure websockets
	// websocket = new WebSocket("wss://chaos-the-game.com/websocket:443");
	
	/*
	this is used for testing on my computer
	*/
	websocket = new WebSocket('ws://localhost:8080');
	
	
	// callback function for when a connection is first established
	websocket.onopen = function() {
		readyToSend = true;
	};
	
	// callback function for new messages
	websocket.onmessage = function(event) {
		var message = event.data;
		
		// split the message into player data and projectile data
		var parts = message.split('/');
		// split the data into individual components using the commas
		var players = parts[0].split(',');
		var walls = parts[1].split(',');
		var projectiles = parts[2].split(',');
		
		var index = 0;
		
		var canvas = document.getElementById("canvas");
		var context = canvas.getContext("2d");
		
		context.clearRect(0, 0, canvas.width, canvas.height);
		
		while (index < players.length) {
			// load the correct image by passing the color of player
			var image = loadImage(players[index]);
			
			// the size of each side of the square
			var size = 100;
			
			// saves the current translation and rotation so it can be restored after drawing
			context.save();
			
			// set the position and rotation of the image
			context.translate(parseInt(players[index+1]), parseInt(players[index+2]));
			context.rotate(parseInt(players[index+3]) * Math.PI/180);
			
			context.drawImage(image, -size/2, -size/2, size, size);
			context.restore();
			index += 4;
		}
		
		index = 0;
		while (index < walls.length) {
			context.save();
			context.translate(parseInt(walls[index]), parseInt(walls[index+1]));
			context.rotate(parseInt(walls[index+2]) * Math.PI / 180);
			context.fillStyle = 'blue';
			context.fillRect(-20, -100, 40, 200);
			
			// draw spikes
			context.beginPath();
			context.moveTo(-20, -100);
			context.lineTo(0, -135);
			context.stroke();
			context.lineTo(20, -100);
			context.stroke();
			
			context.beginPath();
			context.moveTo(-20, 100);
			context.lineTo(0, 135);
			context.stroke();
			context.lineTo(20, 100);
			context.stroke();
			
			context.restore();
			index += 3;
		}
		
		index = 0;
		while (index < projectiles.length) {
			context.beginPath();
			context.arc(parseInt(projectiles[index]), parseInt(projectiles[index+1]), 10, 0, 2 * Math.PI);
			context.stroke();
			index += 2;
		}
	};
	
}

// load the correct sprite for a player based on their color
function loadImage(color) {
	if (color == 'blue') {
		return blueSprite;
	}
	if (color == 'green') {
		return greenSprite;
	}
	if (color == 'purple') {
		return purpleSprite;
	}
	if (color == 'orange') {
		return orangeSprite;
	}
}

document.onkeydown = function(event) {
	keysDown[event.keyCode] = true;
	sendData();
}

document.onkeyup = function(event) {
	keysDown[event.keyCode] = false;
	sendData();
}

function sendData(event) {
	if (!readyToSend) {
		console.log('not ready to send');
		return;
	}
	var data = [0, 0, 0];
	var message = '';
	
	// arrow keys
	if (keysDown[38]) {
		data[1] -= 1;
	}
	if (keysDown[40]) {
		data[1] += 1;
	}
	if (keysDown[37]) {
		data[0] -= 1;
	}
	if (keysDown[39]) {
		data[0] += 1;
	}
	
	// space bar
	if (keysDown[32]) {
		data[2] = 1;
	}
	
	console.log(data);
	
	message = data[0] + ',' + data[1] + ',' + data[2];
	
	websocket.send(message);
}


window.onload = function() {
	var canvas = document.getElementById("canvas");
	var context = canvas.getContext("2d");

	// makes the images not blurry
	context.imageSmoothingEnabled = false;

	/*
	load images
	*/
	/*
	************** used for deployed version
	blueSprite = new Image();
	blueSprite.src = 'player-sprites/player-sprite-blue.png';
	greenSprite = new Image();
	greenSprite.src = 'player-sprites/player-sprite-green.png';
	purpleSprite = new Image();
	purpleSprite.src = 'player-sprites/player-sprite-purple.png';
	orangeSprite = new Image();
	orangeSprite.src = 'player-sprites/player-sprite-orange.png';
	*/
	
	// used for local testing
	blueSprite = new Image();
	blueSprite.src = '../../assets/player-sprites/player-sprite-blue.png';
	greenSprite = new Image();
	greenSprite.src = '../../assets/player-sprites/player-sprite-green.png';
	purpleSprite = new Image();
	purpleSprite.src = '../../assets/player-sprites/player-sprite-purple.png';
	orangeSprite = new Image();
	orangeSprite.src = '../../assets/player-sprites/player-sprite-orange.png';
}



