//
//  drawBehavior.cpp
//  SwarmBots_tempate
//
//  Created by Pierre Dragicevic on 04/04/16.
//
//

#include "DrawBehavior.h"

#define drawSpacing 0.04f // spacing between robots when drawing, robot size included
#define drawingMethod 1 // use 1
#define editMethod 1 // use 0 for translate, use 1 for local deformation
#define deformRadius 2 // number of robots to the left and right that follow the movement during local deformations

ofVec2f drawpos0;
bool drawing = false;
int nextRobot = 1;

void DrawBehavior::init(vector<Zooid>* robots) {
    
    int n = robots->size();
    
    setminrobots(1);
    discard(robots, 1, n-1);
    move(robots, 0, 0.0f, 0.0f);
    
}

void DrawBehavior::update(vector<Zooid>* robots) {

    int n = robots->size();
    
    // ============ DRAW
    
    if (robots->at(0).getState()) {
        ofVec2f pos = robots->at(0).getPosition();
        if (!drawing) {
            drawpos0 = pos;
            drawing = true;
        } else {
            robots->at(0).setDestination(pos);
            float dist = pos.distance(drawpos0);
            if (dist > drawSpacing) {
                
                if (drawingMethod == 0) {
                    
                    // Drawing method #1

                    robots->at(nextRobot).setDestination(drawpos0);
                    if (nextRobot < n)
                        robots->at(nextRobot).setColor(ofColor(0,0,255));
                    drawpos0 = pos;
                    nextRobot++;
                    if (nextRobot == n)
                        nextRobot = 1;
                } else if (drawingMethod == 1) {
                    
                    // Drawing method #2
                    
                    for (int i = min(nextRobot, n-1); i >= 2; i--) {
                        ofVec2f goal = robots->at(i-1).getDestination();
                        robots->at(i).setDestination(goal);
                    }
                    robots->at(1).setDestination(drawpos0);
                    
                    if (nextRobot < n)
                        robots->at(nextRobot).setColor(ofColor(0,0,255));
                    drawpos0 = pos;
                    nextRobot++;
                    
                }
            }
        }
    } else {
        if (drawing)
            drawing = false;
    }
    
    
    // ============ TRANSLATE
    
    if (editMethod == 0) {
        int translating = 0;
        for (int i=1; i < n; i++)
            if (robots->at(i).getState())
                translating = i;
        if (translating > 0) {
            ofVec2f goal = robots->at(translating).getDestination();
            ofVec2f pos = robots->at(translating).getPosition();
            ofVec2f delta = pos - goal;
            for (int i=0; i < n; i++) {
                ofVec2f goali = robots->at(i).getDestination();
                robots->at(i).setDestination(goali + delta);
            }
        }
    }
    
    // ============ LOCAL DEFORMATION
    
    if (editMethod == 1) {
        for (int i=1; i < n; i++) {
            if (robots->at(i).getState()) {
                ofVec2f pos = robots->at(i).getPosition();
                ofVec2f goal = robots->at(i).getDestination();
                ofVec2f deltai = pos - goal;
                for (int j = i - deformRadius; j <= i + deformRadius; j++) {
                    if (j >= 0 && j < n && (j == i || !robots->at(j).getState())) {
                        int d = abs(i-j);
                        float weight = (deformRadius + 1 - d) / (float)(deformRadius + 1);
                        ofVec2f goalj = robots->at(j).getDestination();
                        robots->at(j).setDestination(goalj + weight * deltai);
                    }
                }
            }
        }
    }
    
    
    robots->at(0).setColor(ofColor(255,255,255));

}

