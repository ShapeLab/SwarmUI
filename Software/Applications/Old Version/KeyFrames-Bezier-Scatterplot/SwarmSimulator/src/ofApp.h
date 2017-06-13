#pragma once

#include "ofMain.h"
#include <iostream>
#include <cmath>
#include "HRVO.h"
#include "RobotState.hpp"
#include <math.h>
#include "SwarmReceiver.hpp"
#include "configuration.h"
#include "ofConstants.h"
#include "ofxNetwork.h"

#define OPTIMAL_POSITIONING         1
#define IDENTITY_POSITIONING        0

enum SimulationMode
{
	OFF,
	ON,
	NO_PLANNING
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
	bool runSimulation();
	void sendRobotsOrders();
	void readRobotsPositions();
	void controlRobotSpeed(int id, int8_t motor1, int8_t motor2, ofColor color);
	void controlRobotPosition(uint8_t id, float x, float y, ofColor color, float orientation, float preferredSpeed, bool isFinalGoal);

	void receiveAppGoals();
	void sendPositions();
	 
	void assignGoalIndex(unsigned char mode);

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

private:
    hrvo::Simulator simulator;
    vector<RobotState> robotCollection;
    vector<unsigned int> robotUpdated;
    
	vector<SwarmReceiver*> swarmReceivers;

    ofxUDPManager udpSender;
    ofxUDPManager udpReceiver;
    
    ofxSVG robotArtwork;
    ofxSVG batteryArtwork;
    ofTrueTypeFont myFont;
    
    int goalClicked;
    int robotToCommand;
    
    uint64_t previousSimulationTimestamp;
	SimulationMode  simulationMode;
    
    float kSpeed;
    float prefSpeed;
    float uncertaintyOffset;
    
    bool receiverConnected;

};
