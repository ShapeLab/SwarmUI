#ifndef RobotManager_h
#define RobotManager_h

#include "ofMain.h"
#include "ofxNetwork.h"

#define NO_TOUCH 0

class RobotState {

private:
	unsigned int id;
	float radius;
	float orientation;
	ofVec2f position;
	ofVec2f  goal;
	uint8_t touch;
	bool goalReached;
	ofColor color;
	unsigned int batteryLevel;
    bool reassignable;
public:
	RobotState();
	~RobotState();
	RobotState(unsigned int _id, ofVec2f _position, ofVec2f _goal, ofColor _color, bool _reassignable);
	bool operator==(const RobotState& r);
	bool operator!=(const RobotState& r);
	ofVec2f  getPosition();
	float getOrientation();
	uint8_t getTouch();
	bool isGoalReached();
    bool isReassignable();
	unsigned int getId();
	float getRadius();
    ofVec2f getGoal();
	void setPosition(ofVec2f _pos);
	void setPosition(float _x, float _y);
	void setGoal(ofVec2f _goal);
	void setGoal(float _x, float _y);
	void setColor(ofColor _color);
	void setColor(uint8_t _r, uint8_t _g, uint8_t _b);
	void setId(unsigned int _id);
	void setTouch(uint8_t _touch);
	void setOrientation(float _orientation);
    void setReassignable(bool _reassignable);
};

class RobotManager {

private:
	ofxUDPManager udpSender;
	ofxUDPManager udpReceiver;
	vector<RobotState> robots;
public:
	RobotManager();
	~RobotManager();
	void initialize();
	bool sendRobotUpdates();
	bool receiveRobotInformation();
	vector<RobotState>* getRobots();
	bool updateRobot(unsigned int id, ofVec2f position, float orientation, ofColor color, bool reassignable);
//	bool updateRobot(unsigned int id, ofVec2f position, float orientation);
//	bool updateRobot(unsigned int id, ofVec2f position, ofColor color);
};


#endif //RobotManager_h