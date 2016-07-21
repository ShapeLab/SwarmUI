//
//  drawBehavior.cpp
//  SwarmBots_tempate
//
//  Created by Pierre Dragicevic on 04/04/16.
//
//

#include "BarycenterBehavior.h"

void BarycenterBehavior::init(vector<RobotState>* robots) {
    
    int n = robots->size();
    
    setminrobots(3);
    discard(robots, 4, n-1);
    move(robots, 0, 0.5, 0.5);
    move(robots, 1, 0.4, 0.5);
    move(robots, 2, 0.6, 0.5);
    move(robots, 3, 0.5, 0.3);
    
}

void BarycenterBehavior::update(vector<RobotState>* robots) {
    
    int n = robots->size();
    barycenter(robots, 0, 1, 3);

}


void BarycenterBehavior::barycenter(vector<RobotState>* robots, int bar, int from, int to) {
    
    ofVec2f center;
    
    // compute barycenter for robots from-to
    for (int i = from; i <= to; i++) {
        
        // add to the barycenter
        ofVec2f pos = robots->at(i).getPosition();
        center += pos;
        
        // if a robot is moved, update its goal so it doesn't come back when released
        if (robots->at(i).getTouch())
            robots->at(i).setGoal(pos);
        
        // update color
        robots->at(i).setColor(ofColor(255, 255, 255));
        
    }
    // divide by the number of robots
    center /= (to - from + 1);
    
    // move the robot "bar" to the barycenter
    robots->at(bar).setGoal(center);
    robots->at(bar).setColor(ofColor(0, 0, 255));
    
    
}

