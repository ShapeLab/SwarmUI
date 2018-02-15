// GLOBAL VARS
var socket;
var URL = window.URL ? window.URL : window.webkitURL;
var isOpen = false;

//----------------------------------------------------------------------------------------------------------------
// ON READY, SETUP SOCKET!
//----------------------------------------------------------------------------------------------------------------

$(document).ready( function() {
	document.getElementById("brow").textContent = " " + BrowserDetect.browser + " "
		+ BrowserDetect.version +" " + BrowserDetect.OS +" ";
	setupSocket();
});

//----------------------------------------------------------------------------------------------------------------
// Send form image as blob to server
//----------------------------------------------------------------------------------------------------------------
function sendImage(){
	if (isOpen){
		var file = document.querySelector('input[type="file"]').files[0];
		socket.send(file);
	}
}

//----------------------------------------------------------------------------------------------------------------
// WS:ON OPEN
//----------------------------------------------------------------------------------------------------------------
function onOpen(){
	isOpen = true;
	document.getElementById("wslm_statustd").style.backgroundColor = "#40ff40";
	document.getElementById("wslm_statustd").textContent = " websocket connection opened ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON CLOSE
//----------------------------------------------------------------------------------------------------------------
function onClose(){
	isOpen = false;
	document.getElementById("wslm_statustd").style.backgroundColor = "#ff4040";
	document.getElementById("wslm_statustd").textContent = " websocket connection CLOSED ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON MESSAGE
//----------------------------------------------------------------------------------------------------------------

var prevBlob; 
function onMessage( messageEvent ){
    if (messageEvent.data instanceof Blob) {
		URL.revokeObjectURL(prevBlob);
		prevBlob = URL.createObjectURL(messageEvent.data);
		// just set image.src to the blob URL, then you're all good!
		$("#loadintome").attr("src",prevBlob);
		console.log(prevBlob);
	}
}

//----------------------------------------------------------------------------------------------------------------
// setup web socket
//----------------------------------------------------------------------------------------------------------------

function setupSocket(){
	// setup!
	if (BrowserDetect.browser == "Firefox") {
		socket = new MozWebSocket( get_appropriate_ws_url(), "of-protocol" );
		socket.binaryType = "blob";
	} else {
		//socket = new WebSocket( get_appropriate_ws_url(), "of-protocol");
        socket = new WebSocket( get_appropriate_ws_url(), "of-protocol");
		socket.binaryType = "blob";
	}
	
	// open
	try {
		socket.onopen 		= onOpen;
		socket.onmessage 	= onMessage;
		socket.onclose 	 	= onClose;
	} catch(exception) {
		alert('Error' + exception);  
	}
}