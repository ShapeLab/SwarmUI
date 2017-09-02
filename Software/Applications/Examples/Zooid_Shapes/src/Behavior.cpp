//
//  Behavior.cpp
//  SwarmBots_tempate
//
//  Created by Pierre Dragicevic on 04/04/16.
//
//

#include "Behavior.h"

// 1: top-left grid
// 2: line above
// 3: line above and below, alternatesß
// 4: line above, alternates
#define method 4

#define robotSpacing 0.015f // initial spacing between robots in inactive/waiting mode


void Behavior::init(vector<Zooid>* zooids) {
}

void Behavior::update(vector<Zooid>* zooids) {
}

//
// Move unused myZooids away
//

void Behavior::discard(vector<Zooid>* zooids, int from, int to) {
    
    int n = zooids->size();
    int nd = to - from + 1;
    float s = zooids->at(0).getRadius()*2.0f + robotSpacing;
    
    if (method == 1 || method == 2) {
    
        int cols;
        
        if (method == 1)
            cols = (int)(sqrt(n)+1);
        else
            cols = (int)(dimensionX / (zooids->at(0).getRadius()*2.0f + robotSpacing) - 1);
        
        float x = 0;
        int y = 0;
        
        for (int i = to; i >= from; i--) {
            
            ofVec2f pos;
            
            pos.set(dimensionX - (x + 0.5f) * s, dimensionY - (y + 0.5f) * s);

            zooids->at(i).setDestination(pos);
            
            x += 1;
            if (x >= cols) {
                x = 0;
                y ++;
                if (y%2 == 1) x = 0.5; // add offset
            }
            
            // update color
            zooids->at(i).setColor(ofColor(0, 0, 0));
            
        }
    } else if (method == 3) {
        
        for (int i = to; i >= from; i--) {
            
            float x, y;
        
            int k = i - minrobots;
            
            if (k%2 == 0) {
                // top
                y = dimensionY;
                if ((k/2)%2 == 0) {
                    // center/right
                    x = dimensionX / 2 + s*k/4;
                } else {
                    // left
                    x = dimensionX / 2 - s*(k+2)/4;
                }
            } else {
                // bottom
                y = 0;
                if (((k-1)/2)%2 == 1) {
                    // center/right
                    x = dimensionX / 2 + s*(k+1)/4;
                } else {
                    // left
                    x = dimensionX / 2 - s*(k-1)/4;
                }
            }
            zooids->at(i).setDestination(x, y);
            
            // update color
            zooids->at(i).setColor(ofColor(0, 0, 0));
        }
    } else if (method == 4) {
        
        for (int i = to; i >= from; i--) {
            
            float x, y;
            
            int k = i - minrobots;

            y = dimensionY;
            if (k%2 == 1) {
                // center/right
                x = dimensionX / 2 + s*(k+1)/2;
            } else {
                // left
                x = dimensionX / 2 - s*(k)/2;
            }
            zooids->at(i).setDestination(x, y);
            
            // update color
            zooids->at(i).setColor(ofColor(0, 0, 0));
        }
    }
}

// used in discard
void Behavior::setminrobots(int minrobots_) {
    minrobots = minrobots_;
}

void Behavior::move(vector<Zooid>* robots, int robot, float x, float y) {
    robots->at(robot).setDestination(dimensionX * x, dimensionY * y);
}