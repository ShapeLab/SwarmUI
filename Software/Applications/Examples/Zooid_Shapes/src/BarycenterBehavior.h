#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Behavior.h"


class BarycenterBehavior : public Behavior {
    
public:
    
    void init(vector<Zooid>* zooids);
    
    void update(vector<Zooid>* zooids);

  
protected:
    
    void barycenter(vector<Zooid>* zooids, int bar, int from, int to);
    
};
