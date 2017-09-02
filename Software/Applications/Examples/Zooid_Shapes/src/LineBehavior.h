#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class LineBehavior : public Behavior {
    
public:
    
    void init(vector<Zooid>* robots, int _nbRobots);
    
    void update(vector<Zooid>* robots);

  
protected:
    
    void line(vector<Zooid>* robots);
    void circle(vector<Zooid>* robots);
    void rectangle(vector<Zooid>* robots);
    int nbRobots;
    
};
