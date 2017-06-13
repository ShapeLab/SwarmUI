//
//  RobotState.cpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 27/01/16.
//
//

#include "RobotState.hpp"

RobotState::RobotState() {
	id = 0;
	position = ofVec2f(0.0f, 0.0f);
	radius = 1.0f;
	angle = 0.0f;
	goal = ofVec2f(0.0f, 0.0f);
	touch = NO_TOUCH;
	goalReached = true;
	color = ofColor(0, 0, 0);
	batteryLevel = 100;
}

RobotState::~RobotState() {

}

RobotState::RobotState(unsigned int _id, float _radius, ofVec2f _position, ofVec2f _goal, float _angle, ofColor _color, uint8_t _battery) {
	position = _position;
	radius = _radius;
	angle = _angle;
	goal = _goal;
	touch = NO_TOUCH;
	goalReached = true;
	color = _color;
	batteryLevel = _battery;
	id = _id;
}


bool RobotState::operator==(const RobotState& r)
{
	return  this->id == r.id;
}

bool RobotState::operator!=(const RobotState& r)
{
	return !(*this == r);
}

ofVec2f RobotState::getPosition() {
	return position;
}

float RobotState::getAngle() {
	return angle;
}

ofVec2f RobotState::getGoal() {
	return goal;
}

uint8_t RobotState::getTouch() {
	return touch;
}


bool RobotState::isGoalReached() {
	return goalReached;
}

ofColor RobotState::getColor() {
	return color;
}

unsigned int RobotState::getId() {
	return id;
}

float RobotState::getRadius() {
	return radius;
}

uint8_t RobotState::getBatteryLevel()
{
	return batteryLevel;
}

void RobotState::setPosition(ofVec2f _pos) {
	position = _pos;
}

void RobotState::setPosition(float _x, float _y) {
	position.x = _x;
	position.y = _y;
}

void RobotState::setRadius(float _radius) {
	radius = _radius;
}

void RobotState::setAngle(float _angle) {
	angle = _angle;
}

void RobotState::setGoal(ofVec2f _goal) {
	goal = _goal;
}

void RobotState::setGoal(float _x, float _y) {
	goal.x = _x;
	goal.y = _y;
}

void RobotState::setColor(ofColor _color) {
	color = _color;
}

void RobotState::setGoalReached(bool _goalReached) {
	goalReached = _goalReached;
}

void RobotState::setTouch(uint8_t _touch) {
	touch = _touch;
}

void RobotState::setId(unsigned int _id) {
	id = _id;
}

void RobotState::setBatteryLevel(uint8_t _battery)
{
	batteryLevel = _battery;
}

void RobotState::drawRobot(ofxSVG* artwork, float scale)
{
	ofPushMatrix();
		ofTranslate(position);
		ofRotate(angle, 0.0f, 0.0f, 1.0f);
		ofFill();
		ofSetColor(color);
		ofDrawCircle(0.0f, 0.0f, radius*scale);
		ofPushMatrix();
			ofTranslate(-scale*radius, -scale*radius);
			ofScale(scale*2.0f*radius / artwork->getWidth(), scale*2.0f*radius / artwork->getHeight());
			artwork->draw();
		ofPopMatrix();

		if (touch > 0)
		{
			ofNoFill();
			ofSetColor(ofColor::crimson);
			ofDrawCircle(0.0f, 0.0f, (radius + 0.005f)*scale);
			ofFill();
			ofSetColor(ofColor::crimson, 50);
			ofDrawCircle(0.0f, 0.0f, (radius + 0.005f)*scale);
		}
		if (goalReached)
		{
			ofNoFill();
			ofSetColor(ofColor::lawnGreen);
			ofDrawCircle(0.0f, 0.0f, (radius + 0.005f)*scale);
			ofFill();
			ofSetColor(ofColor::lawnGreen, 50);
			ofDrawCircle(0.0f, 0.0f, (radius + 0.005f)*scale);

		}
	ofPopMatrix();
}

void RobotState::drawGoal()
{
	ofPushMatrix();
	ofSetLineWidth(2.0f);

	ofFill();
	ofSetColor(color, 50);
	ofDrawCircle(goal, radius);

	ofNoFill();
	ofSetColor(0, 50);
	ofDrawCircle(goal, radius);
	ofPopMatrix();
}

void RobotState::drawBatteryLevel(ofxSVG* artwork, float scale)
{
	if (batteryLevel < 40 && batteryLevel > 10)
		ofSetColor(ofColor::darkorange);
	else if (batteryLevel <= 10)
		ofSetColor(ofColor::tomato);
	else
		ofSetColor(ofColor::limeGreen);

	float batteryHeight = ofMap((float)batteryLevel, 0.0f, 100.0f, artwork->getHeight()*0.95f, artwork->getHeight()*0.12f);
	if (batteryHeight > artwork->getHeight()*0.8f)
		batteryHeight = artwork->getHeight()*0.8f;
	ofPushMatrix();
		ofTranslate(position);
		ofFill();
		ofRotate(180.0f, 0.0f, 0.0f, 1.0f);
		ofTranslate(-2.0f*scale*radius, -1.5f*scale*radius);
		ofScale(scale*0.5f*radius / artwork->getWidth(), scale*1.0f*radius / artwork->getHeight());
		ofDrawRectangle(artwork->getWidth()*0.12f, batteryHeight, artwork->getWidth()*0.8f, artwork->getHeight()*0.95f - batteryHeight);
		artwork->draw();
	ofPopMatrix();
}
