#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    client.connect( "localhost", 9092 );
    bConnected = false;
    
    client.addListener(this);

    ofBackground(0);
    ofSetFrameRate(60);
    
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    if ( bConnected ){
        ofDrawBitmapString("WebSocket client connected" , 20, 20);
    } else {
        ofDrawBitmapString("WebSocket not connected", 20,20);
    }
    
    map<int, Drawing*>::iterator it = drawings.begin();
    
    ofNoFill();
    
    for (it; it != drawings.end(); ++it){
        Drawing * d = it->second;
        ofSetColor( d->color );
        ofBeginShape();
        for ( int i=0; i<(int)d->points.size(); i++){
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
    bConnected = true;
    cout<<"new connection open"<<endl;
    cout<<args.conn.getClientIP()<< endl;
    
    // send drawing so far
    map<int, Drawing*>::iterator it = drawings.begin();
    for (it; it != drawings.end(); ++it){
        Drawing * drawing = it->second;
            for ( int i=0; i<(int)drawing->points.size(); i++){
                string x = ofToString(drawing->points[i].x);
                string y = ofToString(drawing->points[i].y);
                client.send( "{\"id\":"+ ofToString(drawing->_id) + ",\"point\":{\"x\":\""+ x+"\",\"y\":\""+y+"\"}," + drawing->getColorJSON() +"}");
            }
    }
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    bConnected = false;
    cout<<"on close"<<endl;
    // remove from color map
    
    map<int, Drawing*>::iterator it = drawings.begin();
    
    for (it; it != drawings.end(); ++it){
        Drawing * d = it->second;
        if ( *d->conn == args.conn ){
            d->conn == NULL;
            client.send("{\"erase\":\"" + ofToString( it->second->_id ) + "\"}" );
            drawings.erase( it );
        }
    }
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
  try{
    cout<<"got message "<<args.message<<endl;
    // trace out string messages or JSON messages!
    if ( !args.json.isNull() ){
        if (!args.json["setup"].isNull()){
            Drawing * d = new Drawing();
            d->_id = args.json["setup"]["id"].asInt();
            // for some reason these come across as strings via JSON.stringify!
            int r = ofToInt(args.json["setup"]["color"]["r"].asString());
            int g = ofToInt(args.json["setup"]["color"]["g"].asString());
            int b = ofToInt(args.json["setup"]["color"]["b"].asString());
            d->color.set(r, g, b);
            drawings.insert( make_pair( d->_id, d ));
            id = d->_id;
            color.set(r, g, b);
            cout << "setup with id:" << id << endl;
        }
        else if (args.json["id"].asInt() != id){
          cout << "received point" << endl;
            ofPoint point = ofPoint( args.json["point"]["x"].asFloat(), args.json["point"]["y"].asFloat() );
            
            // for some reason these come across as strings via JSON.stringify!
            int r = ofToInt(args.json["color"]["r"].asString());
            int g = ofToInt(args.json["color"]["g"].asString());
            int b = ofToInt(args.json["color"]["b"].asString());
            ofColor color = ofColor( r, g, b );
            
            int _id = args.json["id"].asInt();
            
            map<int, Drawing*>::const_iterator it = drawings.find(_id);
            Drawing * d;
            if (it!=drawings.end()){
                d = it->second;
            }
            else {
              d = new Drawing();
              d->_id = _id;
              // for some reason these come across as strings via JSON.stringify!
              int r = ofToInt(args.json["color"]["r"].asString());
              int g = ofToInt(args.json["color"]["g"].asString());
              int b = ofToInt(args.json["color"]["b"].asString());
              d->color.set(r, g, b);
              drawings.insert( make_pair( d->_id, d ));
              cout << "new drawing with id:" << _id << endl;
            }

            d->addPoint(point);
        }
    } else {
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
    
    map<int, Drawing*>::iterator it = drawings.find(id);
    if (it != drawings.end()){
      Drawing * d = it->second;
      d->addPoint(p);
      client.send( "{\"id\":"+ ofToString(id) + ",\"point\":{\"x\":\""+ ofToString(x)+"\",\"y\":\""+ofToString(y)+"\"}," + d->getColorJSON() +"}");
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    ofPoint p(x,y);
    map<int, Drawing*>::iterator it = drawings.find(id);
    if ( it == drawings.end() ) return;
    Drawing * d = it->second;
    d->addPoint(p);
    client.send( "{\"id\":"+ ofToString(id) + ",\"point\":{\"x\":\""+ ofToString(x)+"\",\"y\":\""+ofToString(y)+"\"}," + d->getColorJSON() +"}");
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
