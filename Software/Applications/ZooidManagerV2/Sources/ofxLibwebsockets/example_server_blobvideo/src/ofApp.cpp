#include "ofApp.h"

// quality of image: higher is more readable, lower is faster!
int jpegquality = 100;

//--------------------------------------------------------------
void ofApp::setup(){
    //setup video grabber
    video.listDevices();
    bVideoSetup = video.initGrabber(640,480);
    
    // setup a server with default options on port 9093
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
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if ( bVideoSetup && video.isFrameNew() ){
        static unsigned long size;
        static ofImage currentImage;
        currentImage.setFromPixels(video.getPixels());
        
        // compress video into image via turbojpg
        // the second param == quality. play with this to get a better framerate
        // you can also try resizing your image!
        //currentImage.resize(160, 120);
        unsigned char * compressed = turbo.compress(currentImage,jpegquality,&size);
        server.sendBinary(compressed, size);
        free(compressed);
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
    if ( bVideoSetup ) video.draw(0,0);
      ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 10, 15);
    
    ofSetColor(255);
    ofDrawBitmapString("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL"), 10, 40);
    
    ofDrawBitmapString("Click anywhere to open up browser client", 10, 60);
    ofDrawBitmapString("+/- changes image quality: "+ofToString(jpegquality), 10, 80);
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    messages.push_back("New connection from " + args.conn.getClientIP() );
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
//    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message "<<args.message<<endl;
    
    // trace out string messages or JSON messages!
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
    if ( key == '+'){
        jpegquality ++;
        jpegquality = jpegquality > 100 ? 100 : jpegquality;
    } else if (key == '-'){
        jpegquality--;
        jpegquality = jpegquality < 0 ? 0 : jpegquality;
    }
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
    string url = "http";
    if ( server.usingSSL() ){
        url += "s";
    }
    url += "://localhost:" + ofToString( server.getPort() );
    ofLaunchBrowser(url);
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
void ofApp::dragEvent(ofDragInfo dragInfo){}