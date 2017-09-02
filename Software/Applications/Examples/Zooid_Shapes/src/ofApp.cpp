#include "ofApp.h"
#include "Behavior.h"
#include "DrawBehavior.h"
#include "BarycenterBehavior.h"
#include "LineBehavior.h"


//--------------------------------------------------------------

void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(200, 200, 200);
	zooidManager.initialize(ofGetWidth(), ofGetHeight());
    zooidManager.useRealWorldCoordinates();
}

long iteration = 0;
//DrawBehavior* behavior = new DrawBehavior();
//BarycenterBehavior* behavior = new BarycenterBehavior();
LineBehavior* behavior = new LineBehavior();

//--------------------------------------------------------------
vector<Zooid>* previous_robots = NULL;
void ofApp::update() {
    // acquire new robot data
    if (zooidManager.receiveInformation())
    {
        
        vector<Zooid>* zooids = zooidManager.getZooids();

        if (iteration == 0) {
            behavior->init(zooids, zooids->size());
        } else {
            behavior->update(zooids);
        }

        
//        
//        for (int i=0; i<zooidManager.getNbZooids(); i++) {
//            zooidManager.setZooidOrientation(i, (float)(iteration%360)/10.0f);
//            if (i%3==0)
//                zooidManager.setZooidColor(i, ofColor(iteration%255, 0, 0));
//            else if (i%3==1)
//                zooidManager.setZooidColor(i, ofColor(0, iteration%255, 0));
//            else if (i%3==2)
//                zooidManager.setZooidColor(i, ofColor(0, 0, iteration%255));
//            
//            if (i%2==0)
//                zooidManager.deactivateZooid(i);
//            else
//                zooidManager.activateZooid(i);
//            
//            zooidManager.setZooidReassignable(i, (i%2==0)?true:false);
//        }
//        
        
        //don't forget to send the updates
        zooidManager.sendUpdates();
        
        iteration++;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
   	scaleToRobotWorld();

    vector<Zooid>* zooids = zooidManager.getZooids();
    for(int i=0;i<zooids->size();i++) {
        if(zooids->at(i).getState()==1) ofSetColor(ofColor::red);
        else if(zooids->at(i).getState()==2) ofSetColor(ofColor::yellow);
        else ofSetColor(ofColor::white);
        ofDrawCircle(zooids->at(i).getPosition(), zooids->at(i).getRadius());
    }
	//							 |(0, dimensionY)
	//							 |
	//				your	     |
	//			   screen		 |
	//							 |
	//							 |
	//			_________________|
	//(dimensionX, 0)			 (0, 0)
	//
	//--------------------------------------------
	// Draw your stuff here
	//--------------------------------------------
}

//--------------------------------------------------------------
void ofApp::scaleToRobotWorld() {
	ofScale(ofGetWidth() / dimensionX, ofGetHeight() / dimensionY);
	ofRotate(180.0f, 0.0, 0.0f, 1.0f);
	ofTranslate(-dimensionX, -dimensionY);
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
    float mouseX = dimensionX - (float)x / ofGetWidth()*dimensionX;
    float mouseY = dimensionY - (float)y / ofGetHeight()*dimensionY;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    float mouseX = dimensionX - (float)x / ofGetWidth()*dimensionX;
    float mouseY = dimensionY - (float)y / ofGetHeight()*dimensionY;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
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
