#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class LineBehavior : public Behavior {
    
public:
    
    void init(vector<RobotState>* robots, int _nbRobots);
    
    void update(vector<RobotState>* robots);

  
protected:
    
    void line(vector<RobotState>* robots);
    void circle(vector<RobotState>* robots);
    void rectangle(vector<RobotState>* robots);
    int nbRobots;
    
};
