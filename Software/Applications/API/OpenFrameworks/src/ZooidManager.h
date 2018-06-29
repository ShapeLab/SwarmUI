#ifndef RobotManager_h
#define RobotManager_h

#include "ofMain.h"
#include "ofxNetwork.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/document.h"
#include <iostream>

using namespace rapidjson;
using namespace std;

enum AssignmentMode {
	OptimalAssignment = 1,
	NaiveAssignment = 0
};

class Zooid {

private:

    friend class ZooidManager;
    
	unsigned int id;
	float radius;
	float currentOrientation;
    float targetOrientation;
	ofVec2f currentPosition;
	ofVec2f destination;
    int state;
	bool goalReached;
	ofColor color;
    unsigned int speed;
	bool activated;
	bool reassignable;
    
    bool newOrientation;
    bool newColor;
    bool newDestination;
    bool newSpeed;
    bool newReassignable;
    bool newActivated;
    
public:
	Zooid();
	~Zooid();
	Zooid(unsigned int _id, float _radius, float _orientation, ofVec2f _currentPosition, ofVec2f _destination,
          int _state, bool _goalReached, ofColor _color, bool _activated, bool _reassignable, unsigned int _speed);
	bool operator==(const Zooid& r);
	bool operator!=(const Zooid& r);
	Zooid& operator=(const Zooid& other);

	ofVec2f getPosition();
	ofVec2f getDestination();
	float getOrientation();
    float getTargetOrientation();
    int getState();
    ofColor getColor();
	bool isAtDestination();
    bool isReassignable();
    bool isActivated();
    unsigned int getSpeed();

	unsigned int getId();
	float getRadius();
	void setDestination(ofVec2f _goal);
	void setDestination(float _x, float _y);
	void setColor(ofColor _color);
	void setColor(uint8_t _r, uint8_t _g, uint8_t _b);
	void setOrientation(float _orientation);
    void setReassignable(bool _reassignable);
    void activate();
    void deactivate();
    void setSpeed(unsigned int _speed);

    bool isNewDestination();
    bool isNewOrientation();
    bool isNewColor();
    bool isNewSpeed();
    bool isNewReassignable();
    bool isNewActivated();
    void resetFlags();
};

class ZooidManager {

private:
	ofxUDPManager udpSender;
	ofxUDPManager udpReceiver;
	vector<Zooid> myZooids;
    AssignmentMode assignmentMode;
    bool initialized;
    float dimensionX, dimensionY;
    float windowWidth, windowHeight;
    bool realCoordinates;

public:
	ZooidManager();
	~ZooidManager();
	void initialize(float screenWidth, float screenHeight);
    void initialize(float width, float height, string destinationIP, unsigned int senderPort, unsigned int receiverPort);
	bool sendUpdates();
	bool receiveInformation();
    
	bool updateZooid(unsigned int id, ofVec2f destination, float orientation, ofColor color, bool reassignable);

	bool updateZooid(unsigned int id, ofVec2f destination, float orientation);
	bool updateZooid(unsigned int id, ofVec2f destination, ofColor color);
    bool updateZooid(unsigned int id, ofVec2f destination, ofColor color, float orientation, unsigned int speed, bool reassignable);
    bool updateZooid(unsigned int id, ofColor color, bool activated);
    bool updateZooid(unsigned int id, ofVec2f destination, ofColor color, bool activated);

    bool moveZooid(unsigned id, ofVec2f position);
    bool moveZooid(unsigned id, float x, float y);
 
    void setZooidSpeed(unsigned int id, unsigned int speed);
    unsigned int getZooidSpeed(unsigned int id);

    void setZooidColor(unsigned int id, ofColor c);
    ofColor getZooidColor(unsigned int id);
    
    ofVec2f getZooidPosition(unsigned int id);
    ofVec2f getZooidDestination(unsigned int id);
    float getZooidOrientation(unsigned int id);
    void setZooidOrientation(unsigned int id, float _orientation);

    int getNbZooids();
    
    bool isZooidTouched(unsigned int id);
    bool isZooidBlinded(unsigned int id);
    bool isZooidTapped(unsigned int id);
    bool isZooidShaken(unsigned int id);
    
    bool isZooidActivated(unsigned int id);
    bool isInitialized();
    
    AssignmentMode getAssignmentMode();
    void setAssignmentMode(AssignmentMode mode);

    float getZooidSize(unsigned int id);

    void setZooidReassignable(unsigned int id, bool _reassignable);
    void activateZooid(unsigned int id);
    void deactivateZooid(unsigned int id);
    
    void useRealWorldCoordinates();
    void useWindowCoordinates();
    
    void setWindowSize(float w, float h);
    float getRealWorldWidth();
    float getRealWorldHeight();
    void setRealWorldWidth(float newDimensionX);
    void setRealWorldHeight(float newDimensionY);
    
    vector<Zooid>* getZooids();
    
    int getZooidCaseId(unsigned int id);
};



#endif //RobotManager_h
