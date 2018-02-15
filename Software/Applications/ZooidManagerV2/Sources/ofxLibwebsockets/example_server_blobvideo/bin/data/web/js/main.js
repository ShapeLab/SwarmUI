// GLOBAL VARS
var socket;
var canvas;
var ctx;

var canvasData;
var data;
var type;
var stats;
var image;
var URL = window.URL ? window.URL : window.webkitURL;

//----------------------------------------------------------------------------------------------------------------
// ON READY, SETUP SOCKET!
//----------------------------------------------------------------------------------------------------------------

$(document).ready( function() {
	document.getElementById("brow").textContent = " " + BrowserDetect.browser + " "
		+ BrowserDetect.version +" " + BrowserDetect.OS +" ";
        
	stats = new Stats();
	stats.setMode( 0 ); // 0: fps, 1: ms

	// Align top-left
	stats.domElement.style.position = 'absolute';
	stats.domElement.style.right = '0px';
	stats.domElement.style.top = '0px';

  	document.body.appendChild( stats.domElement );
	setupSocket();
  
 	stats.begin();
    
});

//----------------------------------------------------------------------------------------------------------------
// WS:ON OPEN
//----------------------------------------------------------------------------------------------------------------
function onOpen(){
	document.getElementById("wslm_statustd").style.backgroundColor = "#40ff40";
	document.getElementById("wslm_statustd").textContent = " websocket connection opened ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON CLOSE
//----------------------------------------------------------------------------------------------------------------
function onClose(){
	document.getElementById("wslm_statustd").style.backgroundColor = "#ff4040";
	document.getElementById("wslm_statustd").textContent = " websocket connection CLOSED ";
}

//----------------------------------------------------------------------------------------------------------------
// WS: ON MESSAGE
//----------------------------------------------------------------------------------------------------------------

var prevBlob;
function onMessage( messageEvent ){
    stats.end();
    if (messageEvent.data instanceof Blob) {
      URL.revokeObjectURL(prevBlob);
      prevBlob = URL.createObjectURL(messageEvent.data);
      // just set image.src to the blob URL, then you're all good!
      $("#loadintome").attr("src",prevBlob);
	}
    stats.begin();
}

//----------------------------------------------------------------------------------------------------------------
// setup web socket
//----------------------------------------------------------------------------------------------------------------

function setupSocket(){
	// setup!
	socket = new WebSocket( get_appropriate_ws_url());	
	socket.binaryType = "blob";
	
	// open
	try {
		socket.onopen 		= onOpen;
		socket.onmessage 	= onMessage;
		socket.onclose 	 	= onClose;
	} catch(exception) {
		alert('Error' + exception);  
	}
}
