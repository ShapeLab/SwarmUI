//
// Created by Mathieu Le Goc on 04/02/2017.
//

#ifndef OPENFRAMEWORKS_ROBOTMANAGER_H
#define OPENFRAMEWORKS_ROBOTMANAGER_H


#include "ofMain.h"
#include "HRVO/HRVO.h"
#include <stdio.h>
#include <iostream>
#include "Zooid.h"
#include "ZooidGoal.h"
#include <math.h>

#include "ZooidReceiver.h"
#include "configuration.h"
#include <thread>
#include "ofConstants.h"
#include "ofxNetwork.h"

#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include  "../rapidjson/document.h"

using namespace rapidjson;
using namespace std;

class ZooidManager {
private:
    hrvo::Simulator simulator;
    vector<ZooidGoal> myGoals;
    vector<Zooid> myZooids;

    vector<ZooidReceiver*> myReceivers;
    bool receiverConnected;
    int nbRequiredReceivers;
    ofxUDPManager udpSender;
    ofxUDPManager udpReceiver;

    thread updateThread;
    SimulationMode  simulationMode;
    AssignmentMode assignmentMode;

    float updatePeriod;
    float kSpeed;
    float prefSpeed;
    float uncertaintyOffset;

    bool updating;

    mutex valuesMutex;
    
    ofxSVG zooidArtwork;
    ofxSVG batteryArtwork;
    ofxSVG lockArtwork;
    
    bool initNetwork();
    bool initSimulation();
    bool initZooidReceivers();
    
    void update();
    bool runSimulation();
    bool receiveClientInstructions();
    bool sendClientInformation();
    void readRobotsPositions();
    void sendRobotsOrders();
    void controlRobotSpeed(int id, int8_t motor1, int8_t motor2, ofColor color);
    void controlRobotPosition(uint8_t id, float x, float y, ofColor color, float orientation, float preferredSpeed, bool isFinalGoal);
    ZooidGoal* retrieveAssociatedGoal(unsigned int zooidId);
    ZooidReceiver* retrieveReceiver(unsigned int zooidId);
    
    
public:
    ZooidManager();
    ~ZooidManager();

    void init();
    void assignRobots();

    void drawZooids();
    
    SimulationMode getSimulationMode();
    void setSimulationMode(SimulationMode mode);
    AssignmentMode getAssignmentMode();
    void setAssignmentMode(AssignmentMode mode);
    Zooid getZooid(unsigned int index);
    ZooidGoal getGoal(unsigned int index);

    int getNbGoals();
    int getNbZooids();
    void setZooidInteraction(unsigned int index, bool touched, bool blinded, bool tapped, bool shaken);
    void moveZooid(unsigned index, ofVec2f position);
    void moveZooid(unsigned index, float x, float y);
    void moveGoal(unsigned index, ofVec2f position);
    void moveGoal(unsigned index, float x, float y);

    float getUpdateFrequency();
    bool isReceiverConnected();
    int getNbConnectedReceivers();
    int getNbRequiredReceivers();

        
    bool isZooidTouched(unsigned int id);
    bool isZooidBlinded(unsigned int id);
    bool isZooidTapped(unsigned int id);
    bool isZooidShaken(unsigned int id);
    void addZooid();
    void removeZooid();

};



#endif //OPENFRAMEWORKS_ROBOTMANAGER_H
