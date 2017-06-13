#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "RobotManager.h"

#define dimensionX 0.8128f // table width
#define dimensionY 0.508f  // table height
#define robotSize 0.027f // approximate robot radius

class Behavior {
    
    private: int minrobots;
    
public:

    void init(vector<RobotState>* robots);

    void update(vector<RobotState>* robots);
    
protected:
    
    void discard(vector<RobotState>* robots, int from, int to);
    void setminrobots(int minrobots);
    
    void move(vector<RobotState>* robots, int robot, float x, float y);

};
