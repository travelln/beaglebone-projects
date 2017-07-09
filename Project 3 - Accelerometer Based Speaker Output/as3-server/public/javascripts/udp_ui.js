"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	window.setInterval(function() (pollBeatbox()), 1000);

	$('#btnHihat').click(function(){
		sendCommand("hihat");
	});
	$('#btnSnare').click(function(){
		sendCommand("snare");
	});
	$('#btnBass').click(function(){
		sendCommand("bass");
	});

	$('#btnNone').click(function(){
		sendCommand("mode 0");
	});
	$('#btnMode1').click(function(){
		sendCommand("mode 1");
	});
	$('#btnMode2').click(function(){
		sendCommand("mode 2");
	});
	$('#btnMode3').click(function(){
		sendCommand("mode 3");
	});

	$('#btnVolumeUp').click(function(){
		sendCommand("volumeUp");
	});
	$('#btnVolumeDown').click(function(){
		sendCommand("volumeDown");
	});

	$('#btnTempoUp').click(function(){
		sendCommand("tempoUp");
	});
	$('#btnTempoDown').click(function(){
		sendCommand("tempoDown");
	});


	socket.on('commandReply', function(result) {
		//Get first word of the reply
		var reply = result.substring(0, result.indexOf(' ')); 
		switch(reply) {
			case "volume":
				//console.log(result.substring(result.indexOf(' ')));
				$('#volumeDisplay').text(result.substring(result.indexOf(' ')));
				break;
			case "tempo":
				$('#tempoDisplay').text(result.substring(result.indexOf(' ')));
				break;
			case "uptime":
				var date = new Date(null);
    			date.setSeconds(result.substring(result.indexOf(' ')));
				$('#uptimeDisplay').text(date.toISOString().substr(11, 8));
				break;
			case "mode":
				$('#modeDisplay').text(result.substring(result.indexOf(' ')));
				break;
		}
	//	var newDiv = $('<div></div>').text(result);
	//	$('#messages').append(newDiv);
	//	$('#messages').scrollTop($('#messages').prop('scrollHeight'));
	});
	
});

function sendCommand(message) {
	socket.emit('command', message);
};

function pollBeatbox() {
	socket.emit('command', "pollVolume");
	socket.emit('command', "pollTempo");
	socket.emit('command', "pollMode");
	socket.emit('command', "pollUptime");
}