//
//  drawBehavior.cpp
//  SwarmBots_tempate
//
//  Created by Pierre Dragicevic on 04/04/16.
//
//

#include "BarycenterBehavior.h"

void BarycenterBehavior::init(vector<Zooid>* zooids) {
    
    int n = zooids->size();
    
    setminrobots(3);
    discard(zooids, 4, n-1);
    move(zooids, 0, 0.5, 0.5);
    move(zooids, 1, 0.4, 0.5);
    move(zooids, 2, 0.6, 0.5);
    move(zooids, 3, 0.5, 0.3);
    
}

void BarycenterBehavior::update(vector<Zooid>* zooids) {
    
    int n = zooids->size();
    barycenter(zooids, 0, 1, 3);

}


void BarycenterBehavior::barycenter(vector<Zooid>* zooids, int bar, int from, int to) {
    
    ofVec2f center;
    
    // compute barycenter for myZooids from-to
    for (int i = from; i <= to; i++) {
        
        // add to the barycenter
        ofVec2f pos = zooids->at(i).getPosition();
        center += pos;
        
        // if a robot is moved, update its goal so it doesn't come back when released
        if (zooids->at(i).getState())
            zooids->at(i).setDestination(pos);
        
        // update color
        zooids->at(i).setColor(ofColor(255, 255, 255));
        
    }
    // divide by the number of myZooids
    center /= (to - from + 1);
    
    // move the robot "bar" to the barycenter
    zooids->at(bar).setDestination(center);
    zooids->at(bar).setColor(ofColor(0, 0, 255));
    
    
}

