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

#include "ofxLibwebsockets.h"


using namespace rapidjson;
using namespace std;

#define DEFAULT_UDP_IP "127.0.0.1"
#define DEFAULT_UDP_SENDING_PORT 11999
#define DEFAULT_UDP_RECIEVING_PORT 11998

#define DEFAULT_WEB_PORT 9092

enum class ServerType{
    UDP,
    WEB,
    UDP_WEB,
    NONE
};

class ZooidManagerSettings{
private:
    unsigned int nbZooids;
    unsigned int zooidsPerReceiver;
    
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float areaWidth;
    float areaHeight;
    float unpdateFrequency;
   
};

class ZooidManager {
private:
    unsigned int numZooids;
    hrvo::Simulator simulator;
    vector<ZooidGoal> myGoals;
    vector<Zooid> myZooids;

    ServerType serverType;
    ofxUDPManager udpSender;
    ofxUDPManager udpReceiver;
    string updIPAddress;
    ofxLibwebsockets::Server* webServer;
    unsigned int webServerPort;
    
    bool clientConnected;
    bool bSetup;
    
    vector<ZooidReceiver*> myReceivers;
    bool receiverConnected;
    int nbRequiredReceivers;
    
    thread updateThread;
    mutex valuesMutex;

    SimulationMode simulationMode;
    AssignmentMode assignmentMode;

    float currentUpdatePeriod;
    float kSpeed;
    float prefSpeed;
    float uncertaintyOffset;

    bool updating;

    ofxSVG zooidArtwork;
    ofxSVG batteryArtwork;
    ofxSVG lockArtwork;
    
    ofVec2f worldDimensions;

    void loadParameters();
    void saveParameters();

    bool initNetwork();
    bool initSimulation();
    bool initZooidReceivers();
    
    bool initUDPServer();
    bool initWebServer();
    void closeUDPServer();
    void closeWebServer();
    
    void update();
    bool runSimulation();
    
    bool receiveClientInstructions();
    bool receiveClientInstructions(string webMessage);
    bool processClientInstructions(char* message) ;
    bool sendClientInformation();
    
    void processReceiversData();
    void sendRobotsOrders();
    void controlRobotSpeed(int id, int8_t motor1, int8_t motor2, ofColor color);
    void controlRobotPosition(uint8_t id, float x, float y, ofColor color, float orientation, float preferredSpeed, bool isFinalGoal);
    ZooidGoal* retrieveAssociatedGoal(unsigned int zooidId);
    ZooidReceiver* retrieveReceiver(unsigned int zooidId);
	vector<string> getAvailableZooidReceivers();
    
public:
    ZooidManager();
    ~ZooidManager();

    void init();
    void assignRobots();

    void drawZooids();
    
    ServerType getServerType();
    void setServerType(ServerType type);
    string getUDPIPaddress();
    void setUDPIPaddress(string ipAddress);
    unsigned int getWebServerPort();
    void setWebServerPort(unsigned int port);
    
    SimulationMode getSimulationMode();
    void setSimulationMode(SimulationMode mode);
    AssignmentMode getAssignmentMode();
    void setAssignmentMode(AssignmentMode mode);
  
    float getWorldWidth();
    float getWorldHeight();
    
    void setWorldWidth(float w);
    void setWorldHeight(float h);
    void setWorldDimensions(float w, float h);
    
    Zooid* getZooidFromId(unsigned int zooidId);
    ZooidGoal* getGoalFromId(unsigned int goalId);
    Zooid getZooid(unsigned int index);
    ZooidGoal getGoal(unsigned int index);

    int getNbGoals();
    int getNbZooids();
    void setZooidInteraction(unsigned int index, bool touched, bool blinded, bool tapped, bool shaken);
    void moveZooid(unsigned int index, ofVec2f position);
    void moveZooid(unsigned int index, float x, float y);
    void rotateZooid(unsigned int index, float angle);
    void moveGoal(unsigned int index, ofVec2f position);
    void moveGoal(unsigned int index, float x, float y);

    float getUpdateFrequency();
    bool isReceiverConnected();
    int getNbConnectedReceivers();
    int getNbRequiredReceivers();
        
    bool isZooidTouched(unsigned int index);
    bool isZooidBlinded(unsigned int index);
    bool isZooidTapped(unsigned int index);
    bool isZooidShaken(unsigned int index);
    void addZooid();
    void removeZooid();
    bool isZooidConnected(unsigned int index);

    
    // websocket methods
    void onConnect( ofxLibwebsockets::Event& args );
    void onOpen( ofxLibwebsockets::Event& args );
    void onClose( ofxLibwebsockets::Event& args );
    void onIdle( ofxLibwebsockets::Event& args );
    void onMessage( ofxLibwebsockets::Event& args );
    void onBroadcast( ofxLibwebsockets::Event& args );

};



#endif //OPENFRAMEWORKS_ROBOTMANAGER_H
