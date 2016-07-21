#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class DrawBehavior : public Behavior {
    
public:
    
    void init(vector<RobotState>* robots);
    
    void update(vector<RobotState>* robots);

};
