/*
    Copyright (C) 2015 Stephen Braitsch [http://braitsch.io]

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "ofxDatGuiComponent.h"

bool ofxDatGuiLog::mQuiet = false;
string ofxDatGuiTheme::AssetPath = "";
std::unique_ptr<ofxDatGuiTheme> ofxDatGuiComponent::theme;

ofxDatGuiComponent::ofxDatGuiComponent(string label)
{
    mName = label;
    mVisible = true;
    mEnabled = true;
    mFocused = false;
    mMouseOver = false;
    mMouseDown = false;
    mStyle.opacity = 255;
    this->x = 0; this->y = 0;
    mAnchor = ofxDatGuiAnchor::NO_ANCHOR;
    mLabel.text = label;
    mLabel.alignment = ofxDatGuiAlignment::LEFT;
}

ofxDatGuiComponent::~ofxDatGuiComponent()
{
//  cout << "ofxDatGuiComponent "<< mName << " destroyed" << endl;
    ofRemoveListener(ofEvents().keyPressed, this, &ofxDatGuiComponent::onKeyPressed);
    ofRemoveListener(ofEvents().windowResized, this, &ofxDatGuiComponent::onWindowResized);
}

/*
    instance getters & setters
*/


void ofxDatGuiComponent::setIndex(int index)
{
    mIndex = index;
}

int ofxDatGuiComponent::getIndex()
{
    return mIndex;
}

void ofxDatGuiComponent::setName(string name)
{
    mName = name;
}

string ofxDatGuiComponent::getName()
{
    return mName;
}

bool ofxDatGuiComponent::is(string name)
{
    return ofToLower(mName) == ofToLower(name);
}

ofxDatGuiType ofxDatGuiComponent::getType()
{
    return mType;
}

const ofxDatGuiTheme* ofxDatGuiComponent::getTheme()
{
    if (theme == nullptr) theme = make_unique<ofxDatGuiTheme>(true);
    return theme.get();
}

void ofxDatGuiComponent::setComponentStyle(const ofxDatGuiTheme* theme)
{
    mStyle.height = theme->layout.height;
    mStyle.padding = theme->layout.padding;
    mStyle.vMargin = theme->layout.vMargin;
    mStyle.color.background = theme->color.background;
    mStyle.color.inputArea = theme->color.inputAreaBackground;
    mStyle.color.onMouseOver = theme->color.backgroundOnMouseOver;
    mStyle.color.onMouseDown = theme->color.backgroundOnMouseDown;
    mStyle.stripe.width = theme->stripe.width;
    mStyle.stripe.visible = theme->stripe.visible;
    mStyle.border.width = theme->border.width;
    mStyle.border.color = theme->border.color;
    mStyle.border.visible = theme->border.visible;
    mStyle.guiBackground = theme->color.guiBackground;
    mFont = theme->font.ptr;
    mIcon.y = mStyle.height * .33;
    mIcon.color = theme->color.icons;
    mIcon.size = theme->layout.iconSize;
    mLabel.color = theme->color.label;
    mLabel.margin = theme->layout.labelMargin;
    mLabel.forceUpperCase = theme->layout.upperCaseLabels;
    setLabel(mLabel.text);
    setWidth(theme->layout.width, theme->layout.labelWidth);
    for (int i=0; i<children.size(); i++) children[i]->setTheme(theme);
}

void ofxDatGuiComponent::setWidth(int width, float labelWidth)
{
    mStyle.width = width;
    if (labelWidth > 1){
// we received a pixel value //
        mLabel.width = labelWidth;
    }   else{
// we received a percentage //
        mLabel.width = mStyle.width * labelWidth;
    }
    mIcon.x = mStyle.width - (mStyle.width * .05) - mIcon.size;
    mLabel.rightAlignedXpos = mLabel.width - mLabel.margin;
    for (int i=0; i<children.size(); i++) children[i]->setWidth(width, labelWidth);
    positionLabel();
}

int ofxDatGuiComponent::getWidth()
{
    return mStyle.width;
}

int ofxDatGuiComponent::getHeight()
{
    return mStyle.height;
}

int ofxDatGuiComponent::getX()
{
    return this->x;
}

int ofxDatGuiComponent::getY()
{
    return this->y;
}

void ofxDatGuiComponent::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
    for(int i=0; i<children.size(); i++) children[i]->setPosition(x, this->y + (mStyle.height+mStyle.vMargin)*(i+1));
}

void ofxDatGuiComponent::setVisible(bool visible)
{
    mVisible = visible;
    if (internalEventCallback != nullptr){
        ofxDatGuiInternalEvent e(ofxDatGuiEventType::VISIBILITY_CHANGED, mIndex);
        internalEventCallback(e);
    }
}

bool ofxDatGuiComponent::getVisible()
{
    return mVisible;
}

void ofxDatGuiComponent::setOpacity(float opacity)
{
    mStyle.opacity = opacity * 255;
    for (int i=0; i<children.size(); i++) children[i]->setOpacity(opacity);
}

void ofxDatGuiComponent::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

bool ofxDatGuiComponent::getEnabled()
{
    return mEnabled;
}

void ofxDatGuiComponent::setFocused(bool focused)
{
    if (focused){
        onFocus();
    }   else{
        onFocusLost();
    }
}

bool ofxDatGuiComponent::getFocused()
{
    return mFocused;
}

bool ofxDatGuiComponent::getMouseDown()
{
    return mMouseDown;
}

void ofxDatGuiComponent::setMask(const ofRectangle &mask)
{
    mMask = mask;
}

void ofxDatGuiComponent::setAnchor(ofxDatGuiAnchor anchor)
{
    mAnchor = anchor;
    if (mAnchor != ofxDatGuiAnchor::NO_ANCHOR){
        ofAddListener(ofEvents().windowResized, this, &ofxDatGuiComponent::onWindowResized);
    }   else{
        ofRemoveListener(ofEvents().windowResized, this, &ofxDatGuiComponent::onWindowResized);
    }
    onWindowResized();
}

bool ofxDatGuiComponent::getIsExpanded()
{
	return false;
}

/*
    component label
*/

void ofxDatGuiComponent::setLabel(string label)
{
    mLabel.text = label;
    mLabel.rendered = mLabel.forceUpperCase ? ofToUpper(mLabel.text) : mLabel.rendered = mLabel.text;
    mLabel.rect = mFont->rect(mLabel.rendered);
    positionLabel();
}

string ofxDatGuiComponent::getLabel()
{
    return mLabel.text;
}

void ofxDatGuiComponent::setLabelColor(ofColor c)
{
    mLabel.color = c;
}

ofColor ofxDatGuiComponent::getLabelColor()
{
    return mLabel.color;
}

void ofxDatGuiComponent::setLabelUpperCase(bool toUpper)
{
    mLabel.forceUpperCase = toUpper;
    setLabel(mLabel.text);
}

bool ofxDatGuiComponent::getLabelUpperCase()
{
    return mLabel.forceUpperCase;
}

void ofxDatGuiComponent::setLabelAlignment(ofxDatGuiAlignment align)
{
    mLabel.alignment = align;
    for (int i=0; i<children.size(); i++) children[i]->setLabelAlignment(align);
    positionLabel();
}

void ofxDatGuiComponent::positionLabel()
{
    if (mLabel.alignment == ofxDatGuiAlignment::LEFT){
        mLabel.x = mLabel.margin;
    }   else if (mLabel.alignment == ofxDatGuiAlignment::CENTER){
        mLabel.x = (mLabel.width / 2) - (mLabel.rect.width / 2);
    }   else if (mLabel.alignment == ofxDatGuiAlignment::RIGHT){
        mLabel.x = mLabel.rightAlignedXpos - mLabel.rect.width;
    }
}

/*
    visual customization
*/

void ofxDatGuiComponent::setBackgroundColor(ofColor color)
{
    mStyle.color.background = color;
}

void ofxDatGuiComponent::setBackgroundColorOnMouseOver(ofColor color)
{
    mStyle.color.onMouseOver = color;
}

void ofxDatGuiComponent::setBackgroundColorOnMouseDown(ofColor color)
{
    mStyle.color.onMouseDown = color;
}

void ofxDatGuiComponent::setBackgroundColors(ofColor c1, ofColor c2, ofColor c3)
{
    mStyle.color.background = c1;
    mStyle.color.onMouseOver = c2;
    mStyle.color.onMouseDown = c3;
}

void ofxDatGuiComponent::setStripe(ofColor color, int width)
{
    mStyle.stripe.color = color;
    mStyle.stripe.width = width;
}

void ofxDatGuiComponent::setStripeColor(ofColor color)
{
    mStyle.stripe.color = color;
}

void ofxDatGuiComponent::setStripeWidth(int width)
{
    mStyle.stripe.width = width;
}

void ofxDatGuiComponent::setStripeVisible(bool visible)
{
    mStyle.stripe.visible = visible;
}

void ofxDatGuiComponent::setBorder(ofColor color, int width)
{
    mStyle.border.color = color;
    mStyle.border.width = width;
    mStyle.border.visible = true;
}

void ofxDatGuiComponent::setBorderVisible(bool visible)
{
    mStyle.border.visible = visible;
}

/*
    draw methods
*/

void ofxDatGuiComponent::update(bool acceptEvents)
{
    if (acceptEvents && mEnabled && mVisible){
        bool mp = ofGetMousePressed();
        ofPoint mouse = ofPoint(ofGetMouseX() - mMask.x, ofGetMouseY() - mMask.y);
        if (hitTest(mouse)){
            if (!mMouseOver){
                onMouseEnter(mouse);
            }
            if (!mMouseDown && mp){
                onMousePress(mouse);
                if (!mFocused) onFocus();
            }
        }   else{
    // the mouse is not over the component //
            if (mMouseOver){
                onMouseLeave(mouse);
            }
            if (!mMouseDown && mp && mFocused){
                onFocusLost();
            }
        }
        if (mMouseDown) {
            if (mp){
                onMouseDrag(mouse);
            }   else{
                onMouseRelease(mouse);
            }
        }
    }
// don't update children unless they're visible //
    if (this->getIsExpanded()) {
        for(int i=0; i<children.size(); i++) {
            children[i]->update(acceptEvents);
            if (children[i]->getFocused()){
                if (acceptEvents == false ) children[i]->setFocused(false);
                acceptEvents = false;
            }
        }
    }
}

void ofxDatGuiComponent::draw()
{
    ofPushStyle();
        if (mStyle.border.visible) drawBorder();
        drawBackground();
        drawLabel();
        if (mStyle.stripe.visible) drawStripe();
    ofPopStyle();
}

void ofxDatGuiComponent::drawBackground()
{
    ofFill();
    ofSetColor(mStyle.color.background, mStyle.opacity);
    ofDrawRectangle(x, y, mStyle.width, mStyle.height);
}

void ofxDatGuiComponent::drawLabel()
{
    ofSetColor(mLabel.color);
    if (mType != ofxDatGuiType::DROPDOWN_OPTION){
        mFont->draw(mLabel.rendered, x+mLabel.x, y+mStyle.height/2 + mLabel.rect.height/2);
    }   else{
        mFont->draw("* "+mLabel.rendered, x+mLabel.x, y+mStyle.height/2 + mLabel.rect.height/2);
    }
}

void ofxDatGuiComponent::drawStripe()
{
    ofSetColor(mStyle.stripe.color);
    ofDrawRectangle(x, y, mStyle.stripe.width, mStyle.height);
}

void ofxDatGuiComponent::drawBorder()
{
    ofFill();
    int w = mStyle.border.width;
    ofSetColor(mStyle.border.color, mStyle.opacity);
    ofDrawRectangle(x-w, y-w, mStyle.width+(w*2), mStyle.height+(w*2));
}

void ofxDatGuiComponent::drawColorPicker() { }

/*
    events
*/

bool ofxDatGuiComponent::hitTest(ofPoint m)
{
    if (mMask.height > 0 && (m.y < 0 || m.y > mMask.height)) return false;
    return (m.x>=x && m.x<= x+mStyle.width && m.y>=y && m.y<= y+mStyle.height);
}

void ofxDatGuiComponent::onMouseEnter(ofPoint m)
{
    mMouseOver = true;
}

void ofxDatGuiComponent::onMouseLeave(ofPoint m)
{
    mMouseOver = false;
}

void ofxDatGuiComponent::onMousePress(ofPoint m)
{
    mMouseDown = true;
}

void ofxDatGuiComponent::onMouseRelease(ofPoint m)
{
    mMouseDown = false;
}

void ofxDatGuiComponent::onFocus()
{
    mFocused = true;
    ofAddListener(ofEvents().keyPressed, this, &ofxDatGuiComponent::onKeyPressed);
}

void ofxDatGuiComponent::onFocusLost()
{
    mFocused = false;
    mMouseDown = false;
    ofRemoveListener(ofEvents().keyPressed, this, &ofxDatGuiComponent::onKeyPressed);
}

void ofxDatGuiComponent::onKeyPressed(int key) { }
void ofxDatGuiComponent::onMouseDrag(ofPoint m) { }

void ofxDatGuiComponent::onKeyPressed(ofKeyEventArgs &e)
{
    onKeyPressed(e.key);
    if ((e.key == OF_KEY_RETURN || e.key == OF_KEY_TAB)){
        onFocusLost();
        ofRemoveListener(ofEvents().keyPressed, this, &ofxDatGuiComponent::onKeyPressed);
    }
}

void ofxDatGuiComponent::onWindowResized()
{
    if (mAnchor == ofxDatGuiAnchor::TOP_LEFT){
        setPosition(0, 0);
    }   else if (mAnchor == ofxDatGuiAnchor::TOP_RIGHT){
        setPosition(ofGetWidth()-mStyle.width, 0);
    }
}

void ofxDatGuiComponent::onWindowResized(ofResizeEventArgs &e)
{
    onWindowResized();
}


