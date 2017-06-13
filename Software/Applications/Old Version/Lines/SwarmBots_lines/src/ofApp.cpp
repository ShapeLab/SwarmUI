#include "ofApp.h"
#include "Behavior.h"
#include "DrawBehavior.h"
#include "BarycenterBehavior.h"
#include "LineBehavior.h"

#define NB_ROBOTS 10

//--------------------------------------------------------------

void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(200, 200, 200);
	myRobots.initialize();
    
}

long iteration = 0;
//DrawBehavior* behavior = new DrawBehavior();
//BarycenterBehavior* behavior = new BarycenterBehavior();
LineBehavior* behavior = new LineBehavior();

//--------------------------------------------------------------
vector<RobotState>* previous_robots = NULL;
void ofApp::update() {
    // acquire new robot data
    if (myRobots.receiveRobotInformation())
    {
        
        vector<RobotState>* robots = myRobots.getRobots();
        
        if (iteration == 0) {
            behavior->init(robots, NB_ROBOTS);
        } else {
            behavior->update(robots);
        }
        
        
        //don't forget to send the updates
        myRobots.sendRobotUpdates();
        
        iteration++;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
   	scaleToRobotWorld();
				  
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
