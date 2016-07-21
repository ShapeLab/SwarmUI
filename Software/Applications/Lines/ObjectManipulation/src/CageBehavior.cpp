//
//  drawBehavior.cpp
//  SwarmBots_tempate
//


#include "CageBehavior.h"

#define spacing 0.041f // spacing between robots when interpolating, robot size included
#define minspacing 0.038f //
#define maxspacing 0.065f //

int shape = 0;
int nmid = 8;
int nmidx = 0, nmidy = 0;
float cagecenter = 0.16;
float pho = 180;
float turn1 = 0;
float turn2 = 0;
float section = 180 / (float)(nmid / 2);
ofVec2f o = ofVec2f(0.44, 0.34 + cagecenter);
float d = 0.10;
ofVec2f preaxis = ofVec2f(o.x+d/2, o.y);
int prestart = 2;
void CageBehavior::init(vector<RobotState>* robots) {
    
    int n = robots->size();
    
    setminrobots(2);
    discard(robots, 2, n-1);
    move(robots, 0, 0.03, 0.0);
	move(robots, 1, 0.44, 0.34);
    //move(robots, 1, 0.53, 0.5);
    //move(robots, 2, 0.35, 0.5);
    robots->at(0).setColor(ofColor(255, 0, 0));
    robots->at(1).setColor(ofColor(255, 255, 255));
  
    robots->at(0).setReassignable(false);
    robots->at(1).setReassignable(false);
    //robots->at(2).setReassignable(false);
}

void CageBehavior::update(vector<RobotState>* robots) {
    
    // mode switch
    if (robots->at(0).getTouch()) {
        int old_shape = shape;
        const float offx = 0.05;
        float x = robots->at(0).getPosition().x;
        int mode = min(2, (int)((x - offx) / robotSize));
        x = mode * robotSize + offx;
        robots->at(0).setGoal(x, 0.0);
        if (mode != old_shape) {
            nmid = nmidx = nmidy = 0;
            shape = mode;
        }
    }
    
	caging(robots);
  
}



void CageBehavior::caging(vector<RobotState>* robots) {
    
    int n = robots->size();
    
    // update diameter extremities
    if (robots->at(1).getTouch()) {
        ofVec2f pos = robots->at(1).getPosition();
		//ofVec2f goal = robots->at(1).getGoal();
        robots->at(1).setGoal(pos);
    }
	
 
    ofVec2f p1 = robots->at(1).getGoal();
	ofVec2f goal = ofVec2f(o.x, o.y - cagecenter);
	ofVec2f vec = goal - p1;
	float orient = vec.angle(ofVec2f(1, 0));
	float mag = p1.distance(goal);
	o = ofVec2f(p1.x, p1.y+cagecenter);
	
	ofVec2f left = ofVec2f(p1.x - d / 2, p1.y + cagecenter);
	ofVec2f right = ofVec2f(p1.x + d / 2, p1.y + cagecenter);
	ofVec2f axis = o + (right - o).rotate(-orient);
	int start = 2;
	start = checksection(orient);
	if ((abs(preaxis.angle(axis)) >= 5))
	{
		pho = pho + 2;
		if (pho >= 180)
		{
			pho = 180;
			
		}
		for (int i = 0; i<nmid; i++) {

			//
			if (i < nmid)
			{
				int k = i;
				float a = pho * (nmid / 2 - 1 - k + 0.5) / (float)(nmid / 2);
				ofVec2f p = o + (preaxis - o).rotate(a);
				if ((prestart + i) <= 9)
				{
					robots->at(prestart + i).setGoal(p);
					robots->at(prestart + i).setColor(ofColor(0, 0, 255));
				}
				else
				{
					robots->at(prestart + i - 8).setGoal(p);
					robots->at(prestart + i - 8).setColor(ofColor(0, 0, 255));
				}
			}
			else
			{
				int k = i - (nmid / 2);
				float a = pho * (nmid / 2 - 1 - k + 0.5) / (float)(nmid / 2);
				ofVec2f p_ = o + (preaxis - o).rotate(-a);
				if ((start + i) <= 9)
				{
					robots->at(start + i).setGoal(p_);
					robots->at(start + i).setColor(ofColor(0, 0, 255));
				}
				else
				{
					robots->at(start + i - 8).setGoal(p_);
					robots->at(start + i - 8).setColor(ofColor(0, 0, 255));
				}
			}

		}
	}
	else {
		if (mag > 0)
		{
			pho = pho - 2;
			if (pho <= 130)
			{
				pho = 130;
			}
		}
		else
		{
			pho = pho + 2;
			if (pho >= 180)
			{
				pho = 180;
			}
		}
		for (int i = 0; i<nmid; i++) {

			//
			if (i < nmid)
			{
				int k = i;
				float a = pho * (nmid / 2 - 1 - k + 0.5) / (float)(nmid / 2);
				ofVec2f p = o + (axis - o).rotate(a);
				if ((start + i) <= 9)
				{
					robots->at(start + i).setGoal(p);
					robots->at(start + i).setColor(ofColor(0, 0, 255));
				}
				else
				{
					robots->at(start + i - 8).setGoal(p);
					robots->at(start + i - 8).setColor(ofColor(0, 0, 255));
				}
			}
			else
			{
				int k = i - (nmid / 2);
				float a = pho * (nmid / 2 - 1 - k + 0.5) / (float)(nmid / 2);
				ofVec2f p_ = o + (axis - o).rotate(-a);
				if ((start + i) <= 9)
				{
					robots->at(start + i).setGoal(p_);
					robots->at(start + i).setColor(ofColor(0, 0, 255));
				}
				else
				{
					robots->at(start + i - 8).setGoal(p_);
					robots->at(start + i - 8).setColor(ofColor(0, 0, 255));
				}
			}

		}
	}
		
    // put remaining robots aside
    discard(robots, nmid + 2, n-1);
    
    // translate
    int translating = 0;
    for (int i=2; i < 2+nmid; i++)
        if (robots->at(i).getTouch())
            translating = i;
    if (translating > 0) {
        ofVec2f goal = robots->at(translating).getGoal();
        ofVec2f pos = robots->at(translating).getPosition();
        ofVec2f delta = pos - goal;
        for (int i=1; i < 2+nmid; i++) {
            ofVec2f goali = robots->at(i).getGoal();
            robots->at(i).setGoal(goali + delta);
        }
    }
	preaxis = axis;
	prestart = start;
}


int CageBehavior::checksection(float orient)
{
	if (orient <= section / 2 && orient >-section / 2) { return 2; }
	if (orient <= section*3/ 2 && orient >section / 2) { return 3; }
	if (orient <= section*5/ 2 && orient >section*3/ 2) { return 4; }
	if (orient <= section *7 / 2 && orient >section *5 / 2) { return 5; }
	if (orient <= 180 && orient > section * 7 / 2) { return 6; }
	if (orient <= -section * 7 / 2 && orient >= -180 ) { return 6; }
	if (orient <= -section * 5 / 2 && orient >-section * 7 / 2) { return 7; }
	if (orient <= -section * 3 / 2 && orient >-section * 5 / 2) { return 8; }
	if (orient <= -section / 2 && orient >-section * 3 / 2) { return 9; }
}

