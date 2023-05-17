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
#include "ofxDatGuiTextInputField.h"

class ofxDatGuiTextInput : public ofxDatGuiComponent {

    public:
    
        ofxDatGuiTextInput(string label, string text = "") : ofxDatGuiComponent(label)
        {
            mInput.setText(text);
            mInput.onInternalEvent(this, &ofxDatGuiTextInput::onInputChanged);
            mType = ofxDatGuiType::TEXT_INPUT;
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mStyle.stripe.color = theme->stripe.textInput;
            mInput.setTheme(theme);
            setWidth(theme->layout.width, theme->layout.labelWidth);
        }
    
        void setWidth(int width, float labelWidth)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            mInput.setPosition(x + mLabel.width, y + mStyle.padding);
            mInput.setWidth(mStyle.width - mStyle.padding - mLabel.width);
        }
    
        void setPosition(int x, int y)
        {
            ofxDatGuiComponent::setPosition(x, y);
            mInput.setPosition(x + mLabel.width, y + mStyle.padding);
        }
    
        void setText(string text)
        {
            mInput.setText(text);
        }
    
        string getText()
        {
            return mInput.getText();
        }
    
        void setTextUpperCase(bool toUpper)
        {
            mInput.setTextUpperCase(toUpper);
        }
    
        bool getTextUpperCase()
        {
            return mInput.getTextUpperCase();
        }
    
        void setInputType(ofxDatGuiInputType type)
        {
            mInput.setTextInputFieldType(type);
        }
    
        void draw()
        {
            if (mVisible){
                ofxDatGuiComponent::draw();
                mInput.draw();
            }
        }
    
        bool hitTest(ofPoint m)
        {
            return mInput.hitTest(m);
        }
    
        void dispatchEvent()
        {
            if (textInputEventCallback != nullptr) {
                ofxDatGuiTextInputEvent e(this, mInput.getText());
                textInputEventCallback(e);
            }   else{
                ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
            }
        }
    
        static ofxDatGuiTextInput* getInstance(){ return new ofxDatGuiTextInput("X"); }
    
    protected:

        void onFocus()
        {
            mInput.onFocus();
            ofxDatGuiComponent::onFocus();
        }
    
        void onFocusLost()
        {
            mInput.onFocusLost();
            ofxDatGuiComponent::onFocusLost();
        }
    
        void onKeyPressed(int key)
        {
            if (key != OF_KEY_UP &&
                key != OF_KEY_DOWN)
            mInput.onKeyPressed(key);
        }
    
        virtual void onInputChanged(ofxDatGuiInternalEvent e)
        {
        //  dispatch event out to main application //
            dispatchEvent();
        }
    
        ofxDatGuiTextInputField mInput;
    
};

