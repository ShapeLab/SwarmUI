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

#pragma once
#include "ofxDatGuiComponent.h"

class ofxDatGui2dPad : public ofxDatGuiComponent {

    public:
    
        ofxDatGui2dPad(string label) : ofxDatGuiComponent(label)
        {
            mPercentX = 0.5f;
            mPercentY = 0.5f;
            mType = ofxDatGuiType::PAD2D;
            setTheme(ofxDatGuiComponent::getTheme());
            setBounds(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()), true);
            ofAddListener(ofEvents().windowResized, this, &ofxDatGui2dPad::onWindowResized);
        }
    
        ofxDatGui2dPad(string label, ofRectangle bounds) : ofxDatGuiComponent(label)
        {
            mPercentX = 0.5f;
            mPercentY = 0.5f;
            mType = ofxDatGuiType::PAD2D;
            setTheme(ofxDatGuiComponent::getTheme());
            setBounds(bounds, false);
            ofAddListener(ofEvents().windowResized, this, &ofxDatGui2dPad::onWindowResized);
        }
    
        ~ofxDatGui2dPad()
        {
            ofRemoveListener(ofEvents().windowResized, this, &ofxDatGui2dPad::onWindowResized);
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mStyle.height = theme->layout.pad2d.height;
            mStyle.stripe.color = theme->stripe.pad2d;
            mColors.line = theme->color.pad2d.line;
            mColors.ball = theme->color.pad2d.ball;
            mColors.fill = theme->color.inputAreaBackground;
            mBallSize = theme->layout.pad2d.ballSize;
            mLineWeight = theme->layout.pad2d.lineWeight;
            mPad = ofRectangle(0, 0, mStyle.width - mStyle.padding - mLabel.width, mStyle.height - (mStyle.padding * 2));
        }
    
        void setPoint(ofPoint pt)
        {
            if (mBounds.inside(pt)){
                mPercentX = (pt.x-mBounds.x) / mBounds.width;
                mPercentY = (pt.y-mBounds.y) / mBounds.height;
                setWorldCoordinates();
            }   else{
        //  the point assigment is outside of the 2d pad's bounds //
            }
        }
    
        ofPoint getPoint()
        {
            return mWorld;
        }
    
        void setBounds(ofRectangle bounds, bool scaleOnResize = false)
        {
            mBounds = bounds;
            mScaleOnResize = scaleOnResize;
            setWorldCoordinates();
        }
    
        ofRectangle getBounds()
        {
            return mBounds;
        }
    
        void reset()
        {
            mPercentX = 0.5f;
            mPercentY = 0.5f;
            setWorldCoordinates();
        }
    
        void draw()
        {
            if (!mVisible) return;
            ofPushStyle();
                mPad.x = x + mLabel.width;
                mPad.y = y + mStyle.padding;
                mPad.width = mStyle.width - mStyle.padding - mLabel.width;
                mLocal.x = mPad.x + mPad.width * mPercentX;
                mLocal.y = mPad.y + mPad.height * mPercentY;
                ofxDatGuiComponent::draw();
                ofSetColor(mColors.fill);
                ofDrawRectangle(mPad);
                ofSetLineWidth(mLineWeight);
                ofSetColor(mColors.line);
                ofDrawLine(mPad.x, mLocal.y, mPad.x + mPad.width, mLocal.y);
                ofDrawLine(mLocal.x, mPad.y, mLocal.x, mPad.y + mPad.height);
                ofSetColor(mColors.ball);
                ofDrawCircle(mLocal, mBallSize);
            ofPopStyle();
        }
    
        static ofxDatGui2dPad* getInstance() { return new ofxDatGui2dPad("X"); }
    
    protected:
    
        void setWorldCoordinates()
        {
            mWorld.x = mBounds.x + (mBounds.width * mPercentX);
            mWorld.y = mBounds.y + (mBounds.height * mPercentY);
        }
    
        void onMouseDrag(ofPoint m)
        {
            if (mPad.inside(m)){
                mPercentX = (m.x-mPad.x) / mPad.width;
                mPercentY = (m.y-mPad.y) / mPad.height;
                setWorldCoordinates();
            // dispatch event out to main application //
                if (pad2dEventCallback != nullptr) {
                    ofxDatGui2dPadEvent e(this, mWorld.x, mWorld.y);
                    pad2dEventCallback(e);
                }   else{
                    ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
                }
            }
        }
    
        void onWindowResized(ofResizeEventArgs &e)
        {
        // scale the bounds to the resized window //
            if (mScaleOnResize){
                mBounds.width *= (ofGetWidth() / mBounds.width);
                mBounds.height *= (ofGetHeight() / mBounds.height);
                setWorldCoordinates();
            }
        }
    
    private:
        ofPoint mLocal;
        ofPoint mWorld;
        ofRectangle mPad;
        ofRectangle mBounds;
        float mPercentX;
        float mPercentY;
        int mBallSize;
        int mLineWeight;
        bool mScaleOnResize;
        struct{
            ofColor fill;
            ofColor line;
            ofColor ball;
        } mColors;
    
};
    