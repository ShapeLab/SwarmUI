#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ZooidManager.h"



// frame struct
typedef struct
{
    vector<ofVec2f>  positions;            //< vector of all positions for robot goals
    vector<ofColor>  colors;               //< vector of all colors for robot goals
    vector<float>  orientations;               //< vector of all colors for robot goals
    vector<int> chartNum;
    
} frame;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void scaleToRobotWorld();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void initializeFrame0();
        void addFrame();
        void insertFrame();
        void insertFrameFromRobots();
    
        int touchRobotTest(float x, float y);
    
        void initializeColors();
    
        void initializeCharts();
    
        void sendCurrentFrameAsGoals();
    
        void processOpenFileSelection(ofFileDialogResult openFileResult);
        void saveFrames();
    
private:
	ZooidManager zooidManager;
    bool isUpdated;
    bool isFirstTime;
    vector<frame> frames;
    frame* currentFrame;
    int currentFrameIndex;
    int selectedRobot;
    bool isDraggingRobot;
    
    ofTrueTypeFont myfont;
    vector<ofColor> colorsList;
    
    vector<ofVec2f> scatterPlot1;
    vector<ofVec2f> scatterPlot2;
    vector<ofVec2f> barChart;
    
    ofXml XML;
};


