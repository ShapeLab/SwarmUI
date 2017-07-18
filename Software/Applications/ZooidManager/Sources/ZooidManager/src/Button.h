//
//  Button.h
//  ZooidManager
//
//  Created by Mathieu Le Goc on 6/12/17.
//
//

#ifndef Button_h
#define Button_h

#include <stdio.h>
#include "ofxSvg.h"
#include "ofMain.h"

class Button{
private:
    string text;
    ofRectangle boundingBox;
    ofVec2f position;
    float size;
    bool clicked;
    
    ofTrueTypeFont myFont;
    
public:
    Button();
    Button(string _text, ofVec2f _position, float _size);
    
    void set(string _text, ofVec2f _position, float _size);
    bool isClicked();
    void setClicked(bool _clicked);
    void setText(string _text);
    void draw();
    void draw(ofVec2f _position);
    void setPosition(ofVec2f _position);
    bool isColliding(ofVec2f _mouse);
    
};

#endif /* Button_h */
