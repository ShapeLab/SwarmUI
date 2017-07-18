//
//  Button.cpp
//  ZooidManager
//
//  Created by Mathieu Le Goc on 6/12/17.
//
//

#include "Button.h"


//--------------------------------------------------------------
Button::Button(){
	text = string(" ");
    position.set(0.0f);
    size = 0.0f;
    clicked = false;
    myFont.load("MerriweatherSans-Regular.ttf", 15);
	boundingBox.setPosition(position);
	boundingBox.setSize(1.0f, 1.0f);
}

//--------------------------------------------------------------
Button::Button(string _text, ofVec2f _position, float _size){
	text = string(" ");
    position = _position;
    size = _size;
    boundingBox.setPosition(_position);
    boundingBox.setSize(size, size);
    clicked = false;
    myFont.load("MerriweatherSans-Regular.ttf", 15);

}

//--------------------------------------------------------------
void Button::setText(string _text) {
	text = _text;
}

//--------------------------------------------------------------
void Button::setPosition(ofVec2f _position){
    position = _position;
    boundingBox.setPosition(_position);
}

//--------------------------------------------------------------
void Button::set(string _text, ofVec2f _position, float _size) {
	text = _text;
    position = _position;
    size = _size;
    boundingBox.setPosition(_position);
    boundingBox.setSize(size, size);
}

//--------------------------------------------------------------
bool Button::isColliding(ofVec2f _mouse){
    return boundingBox.inside(_mouse);
}

//--------------------------------------------------------------
bool Button::isClicked() {
    return clicked;
}

//--------------------------------------------------------------
void Button::setClicked(bool _clicked) {
    clicked = _clicked;
}

//--------------------------------------------------------------
void Button::draw(){
    ofFill();
    ofSetColor(128, 143, 143, 100);
    ofDrawRectRounded(position, size, size, 5.0f);
    
    ofNoFill();
    ofSetLineWidth(2.0f);
    ofSetColor(38, 63, 63, 100);
    ofDrawRectRounded(position, size, size, 5.0f);
    myFont.drawString(text,position.x + 0.2f*size,  position.y + 0.9f*size);

    if(clicked){
        ofFill();
        ofSetColor(0, 0, 0, 60);
        ofDrawRectRounded(boundingBox, 5.0f);
    }
}

//--------------------------------------------------------------
void Button::draw(ofVec2f _position){
    setPosition(_position);
    ofFill();
    ofSetColor(128, 143, 143, 100);
    ofDrawRectRounded(position, size, size, 5.0f);
    
    ofNoFill();
    ofSetLineWidth(2.0f);
    ofSetColor(38, 63, 63, 100);
    ofDrawRectRounded(position, size, size, 5.0f);
    myFont.drawString(text,position.x + 0.2f*size,  position.y + 0.9f*size);
    
    if(clicked){
        ofFill();
        ofSetColor(0, 0, 0, 60);
        ofDrawRectRounded(boundingBox, 5.0f);
    }
}
