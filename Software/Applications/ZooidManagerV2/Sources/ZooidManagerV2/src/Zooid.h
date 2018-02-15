//
//  Zooid.hpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 27/01/16.
//
//
#ifndef Robot_h
#define Robot_h

#include "ofMain.h"
#include "ofxSvg.h"
#include "ZooidGoal.h"

#define NO_TOUCH 0


class Zooid {

private:
	unsigned int id;
	float radius;
	float orientation;
	ofVec2f position;
	ZooidGoal* goal;
	unsigned int state;
	bool goalReached;
	ofColor color;
	unsigned int batteryLevel;
    unsigned int speed;
    bool activated;

	void setPosition(ofVec2f _pos);
	void setPosition(float _x, float _y);
	void setRadius(float _radius);
	void setOrientation(float _angle);
	void setGoal(ZooidGoal* _goal);
	void setColor(ofColor _color);
	void setGoalReached(bool _goalReached);
	void setState(unsigned int _state);
	void setId(unsigned int _id);
	void setBatteryLevel(unsigned int _battery);
	void activate();
	void deactivate();
    void setSpeed(unsigned int _speed);
    
public:
	friend class ZooidManager;

	Zooid();
	~Zooid();
	Zooid(float _radius, ofVec2f _position);

    long lastUpdate;
    
	bool operator == (const Zooid& r);
	bool operator != (const Zooid& r);
	void operator = (const Zooid &z);

	ofVec2f  getPosition();
	float getOrientation();
	ZooidGoal* getGoal();
	unsigned int getState();
	bool isGoalReached();
	ofColor getColor();
	unsigned int getId();
	float getRadius();
	unsigned int getBatteryLevel();
    bool isActivated();
    unsigned int getSpeed();

    bool isTouched();
    bool isBlinded();
    bool isTapped();
    bool isShaken();

    void drawRobot(ofxSVG* artwork, float scale);
	void drawBatteryLevel(ofxSVG* artwork, float scale);
};

#endif /* Robot_h */
