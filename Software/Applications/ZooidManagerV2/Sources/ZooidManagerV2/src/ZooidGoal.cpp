#include "ZooidGoal.h"

ZooidGoal::ZooidGoal() {
    id = 0;
    position = ofVec2f(0.0f);
    orientation = 0.0f;
    color = ofColor(0);
    zooid = NULL;
    reassignable = true;
}

ZooidGoal::ZooidGoal(ofVec2f _position, float _orientation, ofColor _color) {
    id = 0;
    position = _position;
    orientation = _orientation;
    color = _color;
    zooid = NULL;
    reassignable = true;
}

ZooidGoal::~ZooidGoal() {
}

void ZooidGoal::operator = (const ZooidGoal &g){
    id = g.id;
    position = g.position;
    orientation = g.orientation;
    color = g.color;
    zooid = g.zooid;
    reassignable = g.reassignable;
}

void ZooidGoal::setId(unsigned int _id) {
    id = _id;
}

void ZooidGoal::setPosition(ofVec2f _position) {
    position = _position;
}

void ZooidGoal::setOrientation(float _orientation) {
    orientation = _orientation;
}

void ZooidGoal::setPosition(float _x, float _y) {
    position.x = _x;
    position.y = _y;
}

void ZooidGoal::setColor(ofColor _color) {
    color = _color;
}

void ZooidGoal::setAssociatedZooid(Zooid *_zooid) {
    zooid = _zooid;
}

void ZooidGoal::setReassignable(bool _reassignable) {
    reassignable = _reassignable;
}

unsigned int ZooidGoal::getId() {
    return id;
}

ofVec2f ZooidGoal::getPosition() {
    return position;
}

float ZooidGoal::getOrientation() {
    return orientation;
}

ofColor ZooidGoal::getColor() {
    return color;
}

Zooid *ZooidGoal::getAssociatedZooid() {
    return zooid;
}

bool ZooidGoal::isReassignable() {
    return reassignable;
}

void ZooidGoal::drawGoal(float size) {
    ofPushMatrix();
    ofSetLineWidth(2.0f);

    ofFill();
    ofSetColor(color, 50);
    ofDrawCircle(position, size);

    ofNoFill();
    ofSetColor(0, 50);
    ofDrawCircle(position, size);
    
    ofDrawLine(position.x, position.y, position.x+size*cos(orientation), position.y+size*sin(orientation));


    ofPopMatrix();
}

void ZooidGoal::drawLock(ofxSVG *artwork, float goalSize, float scale) {
    ofSetLineWidth(2.0f);
    ofFill();
    
    ofPushMatrix();
    ofTranslate(position);
    ofFill();
    ofRotateDeg(180.0f, 0.0f, 0.0f, 1.0f);
    
    ofTranslate(-2.0f * scale * goalSize, 0.5f * scale * goalSize);
    ofScale(scale * 0.5f * goalSize / artwork->getWidth(), scale * 0.75f * goalSize / artwork->getHeight());
    artwork->draw();
    ofPopMatrix();
}
