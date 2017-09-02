#include "ofApp.h"

#define dimensionX 0.8128f
#define dimensionY 0.508f
#define NB_CONTROL_PTS 3
#define robotSpacing 0.02f // initial spacing between robots in inactive/waiting mode
#define robotSize 0.027f // approximate robot radius

//#define NB_ROBOTS 15
//--------------------------------------------------------------
void testApp::setup() {
    ofSetFrameRate(60);
    zooidManager.initialize(ofGetWidth(), ofGetHeight());
    zooidManager.useWindowCoordinates();
    
    isFirstTime = true;
    
    for(int i=0; i<NB_CONTROL_PTS;i++){
        curvePoints.push_back(ofVec2f(dimensionX*(float)i/(float)NB_CONTROL_PTS, dimensionY/(float)NB_CONTROL_PTS));
        if(i<NB_CONTROL_PTS-1){
            curvePoints.push_back(ofVec2f(dimensionX*(float)i/(float)NB_CONTROL_PTS+0.05f, dimensionY/(float)NB_CONTROL_PTS+0.05f));
        }
    }
    
    nbControlPts = 3;
    numRobotsInCurve = 5.0f; // one more than each segment
    
    for(int j=0;j<2*(NB_CONTROL_PTS-1);j+=2){
        // first each part of curve (between two points)
        for( float i = (1.0f/numRobotsInCurve) ; i < 1.0f ; i += (1.0f/numRobotsInCurve) ){
            // The Green Line
            float xa = getPt( curvePoints.at(j).x , curvePoints.at(j+1).x , i );
            float ya = getPt( curvePoints.at(j).y , curvePoints.at(j+1).y , i );
            float xb = getPt( curvePoints.at(j+1).x , curvePoints.at(j+2).x , i );
            float yb = getPt( curvePoints.at(j+1).y , curvePoints.at(j+2).y , i );
            
            // The Black Dot
            float x = getPt( xa , xb , i );
            float y = getPt( ya , yb , i );
            
            curvePoints.push_back(ofVec2f(x, y));
        }
    }
}

//--------------------------------------------------------------
void testApp::update() {
    
    ////////////////////////////////////////////////////////////////////////////////////
    int counter = 5;
    // first part of curve
    for( float i = (1.0f/numRobotsInCurve) ; i < 1.0f ; i += (1.0f/numRobotsInCurve) )
    {
        
        // The Green Line
        float xa = getPt( curvePoints.at(0).x , curvePoints.at(1).x , i );
        float ya = getPt( curvePoints.at(0).y , curvePoints.at(1).y , i );
        float xb = getPt( curvePoints.at(1).x , curvePoints.at(2).x , i );
        float yb = getPt( curvePoints.at(1).y , curvePoints.at(2).y , i );
        
        // The Black Dot
        float x = getPt( xa , xb , i );
        float y = getPt( ya , yb , i );
        
        
        ofVec2f position = ofVec2f(x,y);
        curvePoints.at(counter).set(position);
        
        counter++;
    }
    
    //second point of curve
    for( float i = (1.0f/numRobotsInCurve) ; i < 1.0f ; i += (1.0f/numRobotsInCurve) )
    {
        
        // The Green Line
        float xa = getPt( curvePoints.at(2).x , curvePoints.at(3).x , i );
        float ya = getPt( curvePoints.at(2).y , curvePoints.at(3).y , i );
        float xb = getPt( curvePoints.at(3).x , curvePoints.at(4).x , i );
        float yb = getPt( curvePoints.at(3).y , curvePoints.at(4).y , i );
        
        // The Black Dot
        float x = getPt( xa , xb , i );
        float y = getPt( ya , yb , i );
        
        ofVec2f position = ofVec2f(x,y);
        curvePoints.at(counter).set(position);
        
        counter++;
    }
    
    
    if (zooidManager.receiveInformation()) {
        
        //do things with the new robot positions
        for(int i =0; i < zooidManager.getNbZooids(); i++){
            
            if(i<curvePoints.size()){
                
                if( i < 5){
                    curvePoints[i] = zooidManager.getZooidPosition(i);
                    zooidManager.updateZooid(i, zooidManager.getZooidPosition(i), ofColor::red);
                }
                
                else if (i < (5 + numRobotsInCurve*2)){
                    zooidManager.updateZooid(i, curvePoints[i], ofColor::blue);
                }
            }
            else{
                
                float x, y;
                
                int k = i - 15  ;
                float s = 5.0f*zooidManager.getZooidSize(0);
                
                y = 20.0f;
                if (k%2 == 1) // center/right
                    x = ofGetWidth() / 2 + s*(k+1)/2;
                else // left
                    x = ofGetWidth() / 2 - s*(k)/2;
                
                zooidManager.updateZooid(i, ofVec2f(x,y), ofColor::black);

            }
        }
        zooidManager.setAssignmentMode(AssignmentMode::NaiveAssignment);
        zooidManager.sendUpdates();
    }
}


//--------------------------------------------------------------
void testApp::draw() {
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
}

float testApp::getPt( float n1 , float n2 , float perc )
{
    float diff = n2 - n1;
    
    return n1 + ( diff * perc );
}


