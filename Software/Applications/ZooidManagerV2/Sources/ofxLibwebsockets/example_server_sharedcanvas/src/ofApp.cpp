#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // setup a server with default options on port 9092
    // - pass in true after port to set up with SSL
    //bConnected = server.setup( 9093 );
    
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9092;
    bConnected = server.setup( options );
    
    
    // this adds your app as a listener for the server
    server.addListener(this);
    
    ofBackground(0);
    ofSetFrameRate(60);
    canvasID = 0;
    
    // OF drawing
    
    Drawing * d = new Drawing();
    d->_id = canvasID++;
    d->color.set(ofRandom(255),ofRandom(255),ofRandom(255));
    
    drawings.insert( make_pair( d->_id, d ));
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( toDelete.size() > 0 ){
        for ( auto & i : toDelete ){
            drawings.erase(i->_id);
            
            server.send("{\"erase\":\"" + ofToString( i->_id ) + "\"}" );
        }
        toDelete.clear();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if ( bConnected ){
        ofDrawBitmapString("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL"), 20, 20);
        
        ofSetColor(150);
        ofDrawBitmapString("Click anywhere to open up client example", 20, 40);  
    } else {
        ofDrawBitmapString("WebSocket setup failed :(", 20,20);
    }
    
    map<int, Drawing*>::iterator it = drawings.begin();
    
    ofNoFill();
    
    for (it; it != drawings.end(); ++it){
        Drawing * d = it->second;
        ofSetColor( d->color );
        ofBeginShape();
        for ( int i=0; i<d->points.size(); i++){
            ofVertex( d->points[i].x,d->points[i].y);
        }
        ofEndShape(false);
    }
    ofFill();
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"new connection open"<<endl;
    cout<<args.conn.getClientIP()<< endl;
    
    Drawing * d = new Drawing();
    d->_id = canvasID++;
    d->color.set(ofRandom(255),ofRandom(255),ofRandom(255));;
    d->conn = &( args.conn );
    
    drawings.insert( make_pair( d->_id, d ));
    
    // send "setup"
    args.conn.send( d->getJSONString("setup") );
    
    // send drawing so far
    map<int, Drawing*>::iterator it = drawings.begin();
    for (it; it != drawings.end(); ++it){
        Drawing * drawing = it->second;
        if ( d != drawing ){
            for ( int i=0; i<drawing->points.size(); i++){
                string x = ofToString(drawing->points[i].x);
                string y = ofToString(drawing->points[i].y);
                server.send( "{\"id\":"+ ofToString(drawing->_id) + ",\"point\":{\"x\":\""+ x+"\",\"y\":\""+y+"\"}," + drawing->getColorJSON() +"}");
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
    // remove from color map
    for ( auto & it : drawings){
        Drawing * d = it.second;
        if ( *d->conn == args.conn ){
            toDelete.push_back(it.second);
            d->conn == NULL;
        }
    }
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message "<<args.message<<endl;
    
  try{
    // trace out string messages or JSON messages!
    if ( !args.json.isNull() ){
        ofPoint point = ofPoint( args.json["point"]["x"].asFloat(), args.json["point"]["y"].asFloat() );
        
        // for some reason these come across as strings via JSON.stringify!
        int r = ofToInt(args.json["color"]["r"].asString());
        int g = ofToInt(args.json["color"]["g"].asString());
        int b = ofToInt(args.json["color"]["b"].asString());
        ofColor color = ofColor( r, g, b );
        
        int _id = ofToInt(args.json["id"].asString());
        
        map<int, Drawing*>::const_iterator it = drawings.find(_id);
        Drawing * d = it->second;
        d->addPoint(point);
    } else {
    }
    // send all that drawing back to everybody except this one
    vector<ofxLibwebsockets::Connection *> connections = server.getConnections();
    for ( int i=0; i<connections.size(); i++){
        if ( (*connections[i]) != args.conn ){
            connections[i]->send( args.message );
        }
    }
  }
  catch(exception& e){
    ofLogError() << e.what();
  }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == ' ' ){
        string url = "http";
        if ( server.usingSSL() ){
            url += "s";
        }
        url += "://localhost:" + ofToString( server.getPort() );
        ofLaunchBrowser(url);
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
    ofPoint p(x,y);
    
    map<int, Drawing*>::iterator it = drawings.find(0);
    Drawing * d = it->second;
    d->addPoint(p);
    server.send( "{\"id\":-1,\"point\":{\"x\":\""+ ofToString(x)+"\",\"y\":\""+ofToString(y)+"\"}," + d->getColorJSON() +"}");
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    ofPoint p(x,y);
    
    map<int, Drawing*>::iterator it = drawings.find(0);
    Drawing * d = it->second;
    d->addPoint(p);
    server.send( "{\"id\":-1,\"point\":{\"x\":\""+ ofToString(x)+"\",\"y\":\""+ofToString(y)+"\"}," + d->getColorJSON() +"}");
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

}
