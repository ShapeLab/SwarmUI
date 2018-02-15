#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // setup a server with default options on port 9092
    // - pass in true after port to set up with SSL
    //bool connected = server.setup( 9092 );
    
    // Uncomment this to set up a server with a protocol
    // Right now, clients created via libwebsockets that are connecting to servers
    // made via libwebsockets seem to want a protocol. Hopefully this gets fixed, 
    // but until now you have to do something like this:
    
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    
    bool connected = server.setup( options );
    
    // this adds your app as a listener for the server
    server.addListener(this);
    
    // setup message queue
    
    font.load("myriad.ttf", 20);
    messages.push_back("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL") );
    
    ofBackground(0);
    ofSetFrameRate(60);
    bSendImage = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( bSendImage && toLoad != "" ){
        currentImage.load( toLoad );
        server.send( ofToString(currentImage.getWidth()) +":"+ ofToString( currentImage.getHeight() ) +":"+ ofToString( currentImage.getImageType() ) );
        server.sendBinary( currentImage );
        messages.push_back( "Sending image" );
        bSendImage = false;
        toLoad = "";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int x = font.getSize();
    int y = font.getSize()*2;
    ofSetColor(255);
    for (int i = messages.size() -1; i >= 0; i-- ){
        //font.drawString( messages[i], x, y );
        y += font.stringHeight( messages[i] ) + font.getSize();
    }
    if (currentImage.bAllocated()) currentImage.draw(0,0);
    ofDrawBitmapString("Drag an image onto the window to send!\nOpen your browser to localhost:9093 to receive", 20,20);
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    messages.push_back("New connection from " + args.conn.getClientIP() );
    
    // send the latest image if there is one!
    if ( currentImage.bAllocated() ){
        args.conn.send( ofToString(currentImage.getWidth()) +":"+ ofToString( currentImage.getHeight() ) +":"+ ofToString( currentImage.getImageType() ) );
        args.conn.sendBinary( currentImage );
    }
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message "<<args.message<<endl;
    
    // trace out string messages or JSON messages!
    // args.json is null if badly formed or just not JOSN
    if ( !args.json.isNull() ){
        messages.push_back("New message: " + args.json.toStyledString() + " from " + args.conn.getClientName() );
    } else {
        messages.push_back("New message: " + args.message + " from " + args.conn.getClientName() );
    }
    
    //if (messages.size() > NUM_MESSAGES) messages.erase( messages.begin() );
    
    // echo server = send message right back!
    args.conn.send( args.message );
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofLaunchBrowser("http://localhost:9093");
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    for (int i=0; i<dragInfo.files.size(); i++){
        string file = dragInfo.files[i];
        ofFile f(file); 
        string ex = f.getExtension();
        std::transform(ex.begin(), ex.end(),ex.begin(), ::toupper);
        
        if ( ex == "JPG" || ex == "JPEG" || ex == "PNG" || ex == "GIF" ){
            toLoad = file;
            bSendImage = true;
        }
    }
}