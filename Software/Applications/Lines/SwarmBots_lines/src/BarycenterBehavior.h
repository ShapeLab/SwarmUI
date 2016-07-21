#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class BarycenterBehavior : public Behavior {
    
public:
    
    void init(vector<RobotState>* robots);
    
    void update(vector<RobotState>* robots);

  
protected:
    
    void barycenter(vector<RobotState>* robots, int bar, int from, int to);
    
};
