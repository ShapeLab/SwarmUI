#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ZooidManager.h"

#define dimensionX 0.8128f // table width
#define dimensionY 0.508f  // table height
//#define zooidSize 0.027f // approximate robot radius

class Behavior {
    
    private: int minrobots;
    
public:

    void init(vector<Zooid>* zooids);

    void update(vector<Zooid>* zooids);
    
protected:
    
    void discard(vector<Zooid>* robots, int from, int to);
    void setminrobots(int minrobots);
    
    void move(vector<Zooid>* robots, int robot, float x, float y);

};
