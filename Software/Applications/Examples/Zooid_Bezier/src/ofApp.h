#pragma once
#include "ofMain.h"
#include "ofxNetwork.h"
#include "ZooidManager.h"

// -------------------------------------------------

class testApp : public ofBaseApp {
	
public:
	
	void setup();
	void update();
	void draw();
	
    
    void scaleToRobotWorld();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
    
    float getPt( float n1 , float n2 , float perc );
    

private:
    ZooidManager zooidManager;
    bool isFirstTime;
    
    int nbControlPts;
    float numRobotsInCurve;
    vector <ofVec2f> curvePoints;
    
};

