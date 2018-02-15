#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // connect to either the "server blob" or "server blob video" example!
    client.connect("localhost", 9093);
    
    client.addListener(this);
    ofSetFrameRate(60);
    
    needToLoad = false;
    locked = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( needToLoad ){
        // you can write this directly to a file!
//        ofFile test;
//        test.open("data.jpg", ofFile::WriteOnly);
//        test.writeFromBuffer(buff);
//        test.close();
        
        turbo.load(buff, incoming);
        needToLoad = false;
        locked = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if ( incoming.isAllocated() ){
        incoming.draw(0,0);
    }
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on connected";
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on open";
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on close";
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on idle";
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    if ( locked ) return;
    // need to load this next frame!
    if ( args.isBinary ){
        buff.clear();
        buff.set(args.data.getData(), args.data.size());
        locked = true;
        needToLoad = true;
    } else {
        // got a string message
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    client.send("Hello");
	cout << "sending hello" <<endl;
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
