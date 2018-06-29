//
//  Zooid.cpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 27/01/16.
//
//

#include "Zooid.h"

//--------------------------------------------------------------
Zooid::Zooid() {
    id = 0;
    position = ofVec2f(0.0f, 0.0f);
    radius = 1.0f;
    orientation = 0.0f;
    state = NO_TOUCH;
    goalReached = false;
    color = ofColor(0, 0, 0);
    batteryLevel = 100;
    speed = 100;
    goal = NULL;
    activated = true;
    watchdogCounter = 0;
}

//--------------------------------------------------------------
Zooid::Zooid(float _radius, ofVec2f _position) {
    position = _position;
    radius = _radius;
    orientation = 0.0f;
    state = NO_TOUCH;
    goalReached = false;
    color = ofColor(0);
    batteryLevel = 100;
    speed = 100;
    id = 0;
    goal = NULL;
    activated = true;
    watchdogCounter = 0;
}

//--------------------------------------------------------------
Zooid::~Zooid() {

}

//--------------------------------------------------------------
bool Zooid::operator == (const Zooid &r) {
    return this->id == r.id;
}

//--------------------------------------------------------------
bool Zooid::operator != (const Zooid &r) {
    return !(*this == r);
}

//--------------------------------------------------------------
void Zooid::operator = (const Zooid &z ) {
    id = z.id;
    radius = z.radius;
    orientation = z.orientation;
    position = z.position;
    goal = z.goal;
    state = z.state;
    goalReached = z.goalReached;
    color = z.color;
    batteryLevel = z.batteryLevel;
    activated = z.activated;
}

//--------------------------------------------------------------
ofVec2f Zooid::getPosition() {
    return position;
}

//--------------------------------------------------------------
float Zooid::getOrientation() {
    return orientation;
}

//--------------------------------------------------------------
ZooidGoal *Zooid::getGoal() {
    return goal;
}

//--------------------------------------------------------------
unsigned int Zooid::getState() {
    return state;
}

//--------------------------------------------------------------
bool Zooid::isGoalReached() {
    return goalReached;
}

//--------------------------------------------------------------
ofColor Zooid::getColor() {
    return color;
}

//--------------------------------------------------------------
unsigned int Zooid::getId() {
    return id;
}

//--------------------------------------------------------------
float Zooid::getRadius() {
    return radius;
}

//--------------------------------------------------------------
unsigned int Zooid::getBatteryLevel() {
    return batteryLevel;
}

//--------------------------------------------------------------
bool Zooid::isActivated() {
    return activated;
}

//--------------------------------------------------------------
void Zooid::setPosition(ofVec2f _pos) {
    position = _pos;
}

//--------------------------------------------------------------
void Zooid::setPosition(float _x, float _y) {
    position.x = _x;
    position.y = _y;
}

//--------------------------------------------------------------
void Zooid::setRadius(float _radius) {
    radius = _radius;
}

//--------------------------------------------------------------
void Zooid::setOrientation(float _orientation) {
    orientation = _orientation;
}

//--------------------------------------------------------------
void Zooid::setGoal(ZooidGoal *_goal) {
    goal = _goal;
}

//--------------------------------------------------------------
void Zooid::setColor(ofColor _color) {
    color = _color;
}

//--------------------------------------------------------------
void Zooid::setGoalReached(bool _goalReached) {
    goalReached = _goalReached;
}

//--------------------------------------------------------------
void Zooid::setState(unsigned int _state) {
    state = _state;
}

//--------------------------------------------------------------
void Zooid::setId(unsigned int _id) {
    id = _id;
}

//--------------------------------------------------------------
void Zooid::setBatteryLevel(unsigned int _battery) {
    batteryLevel = _battery;
}

//--------------------------------------------------------------
void Zooid::activate() {
    activated = true;
}

//--------------------------------------------------------------
void Zooid::deactivate() {
    activated = false;
}

//--------------------------------------------------------------
void Zooid::setSpeed(unsigned int _speed) {
    speed = _speed;
    if(speed>100) speed = 100;
}

//--------------------------------------------------------------
unsigned int Zooid::getSpeed() {
    return speed;
}

//--------------------------------------------------------------
void Zooid::tickWatchdog() {
    if(watchdogCounter != 0)
        watchdogCounter--;
    else{
        batteryLevel = 100;
    }
}

//--------------------------------------------------------------
void Zooid::resetWatchdog() {
    watchdogCounter = ZOOID_WATCHDOG_TIMEOUT;
}

//--------------------------------------------------------------
bool Zooid::isConnected() {
    return watchdogCounter > 0;
}

//--------------------------------------------------------------
bool Zooid::isTouched() {
    return (state & 1) > 0;
}

//--------------------------------------------------------------
bool Zooid::isBlinded() {
    return (state & 2) > 0;
}

//--------------------------------------------------------------
bool Zooid::isTapped() {
    return (state & 4) > 0;
}

//--------------------------------------------------------------
bool Zooid::isShaken() {
    return (state & 8) > 0;
}

//--------------------------------------------------------------
void Zooid::drawRobot(ofxSVG *artwork, float scale) {
    ofSetLineWidth(2.0f);   
    ofPushMatrix();
    ofTranslate(position);
    ofRotate(orientation, 0.0f, 0.0f, 1.0f);
    ofFill();
    ofSetColor(color);
    ofDrawCircle(0.0f, 0.0f, radius * scale);
    ofPushMatrix();
    ofTranslate(-scale * radius, -scale * radius);
    ofScale(scale * 2.0f * radius / artwork->getWidth(), scale * 2.0f * radius / artwork->getHeight());
    artwork->draw();
    ofPopMatrix();

    float size = (radius + 0.005f) * scale;
    if (isTouched()) {
        ofNoFill();
        ofSetColor(ofColor::crimson);
        ofDrawCircle(0.0f, 0.0f, size);
        ofFill();
        ofSetColor(ofColor::crimson, 50);
        ofDrawCircle(0.0f, 0.0f, size);
    }
    
    else if (!activated || isBlinded()) {

//        ofFill();
//        ofSetColor(ofColor::lightGrey, 175);
//        ofDrawCircle(0.0f, 0.0f, size);
        ofNoFill();
        ofSetColor(ofColor::darkGray, 235);
        ofSetLineWidth(4.0f);
        ofDrawLine(-size/2.0f, size/2.0f, size/2.0f, -size/2.0f);
        ofDrawLine(-size/2.0f, -size/2.0f, size/2.0f, size/2.0f);
    }
//    else if (goalReached) {
//        ofNoFill();
//        ofSetColor(ofColor::lawnGreen);
//        ofDrawCircle(0.0f, 0.0f, size);
//        ofFill();
//        ofSetColor(ofColor::lawnGreen, 50);
//        ofDrawCircle(0.0f, 0.0f, size);
//    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void Zooid::drawBatteryLevel(ofxSVG *artwork, float scale) {
    if (batteryLevel < 40 && batteryLevel > 10)
        ofSetColor(ofColor::darkorange);
    else if (batteryLevel <= 10)
        ofSetColor(ofColor::tomato);
    else
        ofSetColor(ofColor::limeGreen);

    float batteryHeight = ofMap((float) batteryLevel, 0.0f, 100.0f, artwork->getHeight() * 0.95f,
                                artwork->getHeight() * 0.12f);
    if (batteryHeight > artwork->getHeight() * 0.8f)
        batteryHeight = artwork->getHeight() * 0.8f;
    ofPushMatrix();
    ofTranslate(position);
    ofFill();
    ofRotate(180.0f, 0.0f, 0.0f, 1.0f);
    ofTranslate(-2.0f * scale * radius, -1.5f * scale * radius);
    ofScale(scale * 0.5f * radius / artwork->getWidth(), scale * 1.0f * radius / artwork->getHeight());
    ofDrawRectangle(artwork->getWidth() * 0.12f, batteryHeight, artwork->getWidth() * 0.8f,
                    artwork->getHeight() * 0.95f - batteryHeight);
    artwork->draw();
    ofPopMatrix();
}
