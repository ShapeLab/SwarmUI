//
//  drawBehavior.cpp
//  SwarmBots_tempate
//
//  Created by Pierre Dragicevic on 04/04/16.
//
//

#include "LineBehavior.h"

#define spacing 0.041f // spacing between robots when interpolating, robot size included
#define minspacing 0.038f //
#define maxspacing 0.065f //

int shape = 0;
int nmid = 0;
int nmidx = 0, nmidy = 0;

void LineBehavior::init(vector<Zooid>* robots, int _nbRobots) {
    nbRobots = _nbRobots;
    
    
    setminrobots(3);
    discard(robots, 3, nbRobots-1);
    move(robots, 0, 0.05, 0.0);
    move(robots, 1, 0.47, 0.5);
    move(robots, 2, 0.53, 0.5);

}

void LineBehavior::update(vector<Zooid>* robots) {
    // mode switch

    robots->at(0).setColor(ofColor(255, 0, 0));
    robots->at(1).setColor(ofColor(255, 255, 255));
    robots->at(2).setColor(ofColor(255, 255, 255));
    robots->at(0).setReassignable(false);
    robots->at(1).setReassignable(false);
    robots->at(2).setReassignable(false);
//    robots->at(0).deactivate();

    if (robots->at(0).getState()) {

        int old_shape = shape;
        const float offx = 0.05;
        float x = robots->at(0).getPosition().x;
        int mode = min(2, (int)((x - offx) / robots->at(0).getRadius()*2.0f));
        if(mode<0) mode = 0;
        x = mode * robots->at(0).getRadius()*2.0f + offx;
        robots->at(0).setDestination(x, 0.0);
        if (mode != old_shape) {
            nmid = nmidx = nmidy = 0;
            shape = mode;
        }
    }
    
    // draw shape
    if (shape == 0) {
        line(robots);
    } else if (shape == 1) {
        circle(robots);
    }
    else if (shape == 2) {
        rectangle(robots);
    }
}

void LineBehavior::line(vector<Zooid>* robots) {

    // update line extremities
    if (robots->at(1).getState())
        robots->at(1).setDestination(robots->at(1).getPosition());

    if (robots->at(2).getState())
        robots->at(2).setDestination(robots->at(2).getPosition());

    // get line extremities
    ofVec2f p1 = robots->at(1).getDestination();
    ofVec2f p2 = robots->at(2).getDestination();

    // count number of needed myZooids
    float d = p1.distance(p2);
    float current_spacing = d / (nmid+1);
    if (current_spacing < minspacing || current_spacing > maxspacing)
        nmid = max(0, min(nbRobots-3, (int)(d / spacing) - 1));

    // update interpolating myZooids
    for (int i=0; i<nmid; i++) {
        
        // insert/remove in the middle
        int k;
        if (i%2 == 0)
            k = i/2;
        else
            k = (nmid-1) - (i-1)/2;
        
        float t = (k+1) / (float)(nmid + 1);
        ofVec2f p = p1 + t * (p2 - p1);
        robots->at(3 + i).setDestination(p);
        robots->at(3+i).setColor(ofColor(0, 0, 255));
        robots->at(3+i).setReassignable(true);
    }
    // put remaining myZooids aside
    discard(robots, nmid + 3, robots->size()-1);

    // translate
    int translating = 0;
    for (int i=3; i < 3+nmid; i++)
        if (robots->at(i).getState())
            translating = i;
    if (translating > 0) {
        ofVec2f goal = robots->at(translating).getDestination();
        ofVec2f pos = robots->at(translating).getPosition();
        ofVec2f delta = pos - goal;
        for (int i=1; i < 3+nmid; i++) {
            ofVec2f goali = robots->at(i).getDestination();
            robots->at(i).setDestination(goali + delta);
        }
    }
}

void LineBehavior::circle(vector<Zooid>* robots) {
    

    // update diameter extremities
    if (robots->at(1).getState()) {
        ofVec2f pos = robots->at(1).getPosition();
        robots->at(1).setDestination(pos);
    }
    if (robots->at(2).getState()) {
        ofVec2f pos = robots->at(2).getPosition();
        robots->at(2).setDestination(pos);
    }
    
    // get diameter extremities
    ofVec2f p1 = robots->at(1).getDestination();
    ofVec2f p2 = robots->at(2).getDestination();

    // count number of needed myZooids
    float d = PI * p1.distance(p2);
    float current_spacing = d / (nmid+3);
    if (current_spacing < minspacing || current_spacing > maxspacing) {
        nmid = max(0, min(nbRobots-3, (int)(d / spacing) - 3));
        nmid = 2 * (int)(nmid/2); // has to be even
    }

    // update interpolating myZooids
    ofVec2f o = (p1+p2)/2;
    for (int i=0; i<nmid/2; i++) {
        
        //
        int k = i;
        
        float a = 180 * (k+1) / (float)(nmid/2 + 1);
        ofVec2f p = o + (p2 - o).rotate(a);
        robots->at(3 + i * 2).setDestination(p);
        robots->at(3 + i*2).setColor(ofColor(0, 0, 255));
        ofVec2f p_ = o + (p1 - o).rotate(a);
        robots->at(3 + i * 2 + 1).setDestination(p_);
        robots->at(3 + i*2 + 1).setColor(ofColor(0, 0, 255));
    }
    // put remaining myZooids aside
    discard(robots, nmid + 3, robots->size()-1);
    
    // translate
    int translating = 0;
    for (int i=3; i < 3+nmid; i++)
        if (robots->at(i).getState())
            translating = i;
    if (translating > 0) {
        ofVec2f goal = robots->at(translating).getDestination();
        ofVec2f pos = robots->at(translating).getPosition();
        ofVec2f delta = pos - goal;
        for (int i=1; i < 3+nmid; i++) {
            ofVec2f goali = robots->at(i).getDestination();
            robots->at(i).setDestination(goali + delta);
        }
    }
    
}


void LineBehavior::rectangle(vector<Zooid>* robots) {
    
    // update rectangle extremities
    if (robots->at(1).getState()) {
        ofVec2f pos = robots->at(1).getPosition();
        robots->at(1).setDestination(pos);
    }
    if (robots->at(2).getState()) {
        ofVec2f pos = robots->at(2).getPosition();
        robots->at(2).setDestination(pos);
    }
    
    // get rectangle extremities
    ofVec2f p1 = robots->at(1).getPosition();
    ofVec2f p2 = robots->at(2).getPosition();
    ofVec2f p3 = ofVec2f(p1.x, p2.y);
    ofVec2f p4 = ofVec2f(p2.x, p1.y);
    
    // count number of needed myZooids for each side
    float dx = p1.distance(p4);
    float dy = p1.distance(p3);
    if (dx < minspacing || dy < minspacing) {
        nmidx = 0;
        nmidy = 0;
        nmid = 0;
    } else {
        float current_spacing_y = dy / (nmidy+1);
        if (current_spacing_y < minspacing || current_spacing_y > maxspacing)
            nmidy = max(0, (int)(dy / spacing) - 1);
        float current_spacing_x = dx / (nmidx+1);
        if (current_spacing_x < minspacing || current_spacing_x > maxspacing)
            nmidx = max(0, (int)(dx / spacing) - 1);
        nmid = 2 + 2 * nmidx + 2 * nmidy;
        if (nmid > nbRobots-3) {
            nmidx = (int)((nbRobots-5)*(dx/(dx+dy))/2);
            nmidy = (int)((nbRobots-5)*(dy/(dx+dy))/2);
            nmid = 2 + 2 * nmidx + 2 * nmidy;
        }
    }
    
    // update interpolating myZooids
    // the two missing corners
    if (nmid > 0) {
        robots->at(3).setDestination(p3);
        robots->at(3).setColor(ofColor(0, 0, 255));
        robots->at(4).setDestination(p4);
        robots->at(4).setColor(ofColor(0, 0, 255));
    }
    // the others
    for (int i=0; i<nmidx; i++) {
        float t = (i+1) / (float)(nmidx + 1);
        robots->at(5 + i * 2).setDestination(p1 + t * (p4 - p1));
        robots->at(5+i*2).setColor(ofColor(0, 0, 255));
        robots->at(5 + i * 2 + 1).setDestination(p3 + t * (p4 - p1));
        robots->at(5+i*2+1).setColor(ofColor(0, 0, 255));
    }
    for (int i=0; i<nmidy; i++) {
        float t = (i+1) / (float)(nmidy + 1);
        robots->at(5 + nmidx * 2 + i * 2).setDestination(p1 + t * (p3 - p1));
        robots->at(5+nmidx*2+i*2).setColor(ofColor(0, 0, 255));
        robots->at(5 + nmidx * 2 + i * 2 + 1).setDestination(p4 + t * (p3 - p1));
        robots->at(5+nmidx*2+i*2+1).setColor(ofColor(0, 0, 255));
    }
    // put remaining myZooids aside
    discard(robots, nmid + 3, robots->size()-1);
    
    // translate
    int translating = 0;
    for (int i=3; i < 3+nmid; i++)
        if (robots->at(i).getState())
            translating = i;
    if (translating > 0) {
        ofVec2f goal = robots->at(translating).getDestination();
        ofVec2f pos = robots->at(translating).getPosition();
        ofVec2f delta = pos - goal;
        for (int i=1; i < 3+nmid; i++) {
            ofVec2f goali = robots->at(i).getDestination();
            robots->at(i).setDestination(goali + delta);
        }
    }
    
}


