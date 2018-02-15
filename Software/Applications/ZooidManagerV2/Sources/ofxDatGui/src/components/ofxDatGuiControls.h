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
#include "ofxDatGuiButton.h"

class ofxDatGuiHeader : public ofxDatGuiButton {

    public:

        ofxDatGuiHeader(string label, bool draggable = true) : ofxDatGuiButton(label)
        {
            mDraggable = draggable;
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mLabel.width = mStyle.width;
            mStyle.stripe.visible = false;
            mStyle.height = mStyle.height * .8;
            setLabelAlignment(ofxDatGuiAlignment::CENTER);
        }
    
        void setDraggable(bool draggable)
        {
            mDraggable = draggable;
        }
    
        bool getDraggable()
        {
            return mDraggable;
        }
    
        ofPoint getDragOffset()
        {
            return mDragOffset;
        }
    
    protected:

        void onMouseEnter(ofPoint m)
        {
        // disable hover state if we're not draggable //
            if (mDraggable) ofxDatGuiComponent::onMouseEnter(m);
        }
    
        void onMousePress(ofPoint m)
        {
            mDragOffset = ofPoint(m.x-this->x,m.y-this->y);
            ofxDatGuiComponent::onMousePress(m);
        }
    
        void onMouseRelease(ofPoint m)
        {
            mDragOffset = m;
            ofxDatGuiComponent::onFocusLost();
            ofxDatGuiComponent::onMouseRelease(m);
        }
    
    // allow panel to be dragged around //
        void onFocusLost() {}
    // force header label to always be centered //
        void setLabelAlignment(ofxDatGuiAlignment align)
        {
            ofxDatGuiComponent::setLabelAlignment(ofxDatGuiAlignment::CENTER);
        }
    
    private:
        bool mDraggable;
        ofPoint mDragOffset;

};

class ofxDatGuiFooter : public ofxDatGuiButton {

    public:
    
        ofxDatGuiFooter() : ofxDatGuiButton("collapse controls")
        {
            mGuiExpanded = true;
            mLabelCollapsed = "expand controls";
            mLabelExpanded = "collapse controls";
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mLabel.width = mStyle.width;
            mStyle.stripe.visible = false;
            mStyle.height = mStyle.height * .8;
            setLabelAlignment(ofxDatGuiAlignment::CENTER);
        }
    
        void setLabelWhenExpanded(string label)
        {
            mLabelExpanded = label;
            if (mGuiExpanded) setLabel(mLabelExpanded);
        }
    
        void setLabelWhenCollapsed(string label)
        {
            mLabelCollapsed = label;
            if (!mGuiExpanded) setLabel(mLabelCollapsed);
        }
    
//        void setY(int y)
//        {
//            this->y = y;
//        }
    
        void setExpanded(bool expanded)
        {
            mGuiExpanded = expanded;
            if (mGuiExpanded){
                setLabel(mLabelExpanded);
            }   else{
                setLabel(mLabelCollapsed);
            }
        }
    
    protected:
    
        void onMouseRelease(ofPoint m)
        {
            ofxDatGuiComponent::onMouseRelease(m);
        // dispatch event out to main application //
            ofxDatGuiInternalEvent e(ofxDatGuiEventType::GUI_TOGGLED, mIndex);
            internalEventCallback(e);
        }
    
    // force footer label to always be centered //
        void setLabelAlignment(ofxDatGuiAlignment align)
        {
            ofxDatGuiComponent::setLabelAlignment(ofxDatGuiAlignment::CENTER);
        }
    
    private:
        bool mGuiExpanded;
        string mLabelExpanded;
        string mLabelCollapsed;
    
};
