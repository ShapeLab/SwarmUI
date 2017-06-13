#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class CageBehavior : public Behavior {
    
public:
    
    void init(vector<RobotState>* robots);
    
    void update(vector<RobotState>* robots);

  
protected:
    
	void caging(vector<RobotState>* robots);
	int  checksection(float);
    
};
