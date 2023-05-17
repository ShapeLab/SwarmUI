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
#include "ofxDatGuiIntObject.h"

class ofxDatGuiTextInputField : public ofxDatGuiInteractiveObject{

    public:
    
        ofxDatGuiTextInputField()
        {
            mFocused = false;
            mTextChanged = false;
            mHighlightText = false;
            mMaxCharacters = 99;
            mType = ofxDatGuiInputType::ALPHA_NUMERIC;
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        void setWidth(int w)
        {
            mInputRect.width = w;
        }
    
        void setPosition(int x, int y)
        {
            mInputRect.x = x;
            mInputRect.y = y;
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            mFont = theme->font.ptr;
            mInputRect.height = theme->layout.height - (theme->layout.padding * 2);
            color.active.background = theme->color.textInput.backgroundOnActive;
            color.inactive.background = theme->color.inputAreaBackground;
            color.active.text = theme->color.label;
            color.inactive.text = theme->color.textInput.text;
            color.highlight = theme->color.textInput.highlight;
            mUpperCaseText = theme->layout.textInput.forceUpperCase;
            mHighlightPadding = theme->layout.textInput.highlightPadding;
            setText(mText);
        }
    
        void draw()
        {
        // center the text //
            int tx = mInputRect.x + mInputRect.width / 2 - mTextRect.width / 2;
            int ty = mInputRect.y + mInputRect.height / 2 + mTextRect.height / 2;
            ofPushStyle();
            // draw the input field background //
                if (mFocused && mType != ofxDatGuiInputType::COLORPICKER){
                    ofSetColor(color.active.background);
                }   else {
                    ofSetColor(color.inactive.background);
                }
                ofDrawRectangle(mInputRect);
            // draw the highlight rectangle //
                if (mHighlightText){
                    ofRectangle hRect;
                    hRect.x = tx - mHighlightPadding;
                    hRect.width = mTextRect.width + (mHighlightPadding * 2);
                    hRect.y = ty - mHighlightPadding - mTextRect.height;
                    hRect.height = mTextRect.height + (mHighlightPadding * 2);
                    ofSetColor(color.highlight);
                    ofDrawRectangle(hRect);
                }
            // draw the text //
                ofColor tColor = mHighlightText ? color.active.text : color.inactive.text;
                ofSetColor(tColor);
                mFont->draw(mType == ofxDatGuiInputType::COLORPICKER ? "#" + mRendered : mRendered, tx, ty);
                if (mFocused) {
            // draw the cursor //
                    ofDrawLine(ofPoint(tx + mCursorX, mInputRect.getTop()), ofPoint(tx + mCursorX, mInputRect.getBottom()));
                }
            ofPopStyle();
        }
    
        int getWidth()
        {
            return mInputRect.width;
        }
    
        int getHeight()
        {
            return mInputRect.height;
        }
    
        bool hasFocus()
        {
            return mFocused;
        }
    
        bool hitTest(ofPoint m)
        {
            return (m.x>=mInputRect.x && m.x<=mInputRect.x+mInputRect.width && m.y>=mInputRect.y && m.y<=mInputRect.y+mInputRect.height);
        }
    
        void setText(string text)
        {
            mText = text;
            mTextChanged = true;
            mRendered = mUpperCaseText ? ofToUpper(mText) : mText;
            mTextRect = mFont->rect(mType == ofxDatGuiInputType::COLORPICKER ? "#" + mRendered : mRendered);
        }
    
        string getText()
        {
            return mText;
        }
    
        void setTextActiveColor(ofColor c)
        {
            color.active.text = c;
        }
    
        void setTextInactiveColor(ofColor c)
        {
            color.inactive.text = c;
        }
    
        void setTextUpperCase(bool toUpper)
        {
            mUpperCaseText = toUpper;
            setText(mText);
        }
    
        bool getTextUpperCase()
        {
            return mUpperCaseText;
        }
    
        void setTextInputFieldType(ofxDatGuiInputType type)
        {
            mType = type;
        }
    
        void setBackgroundColor(ofColor c)
        {
            color.inactive.background = c;
        }
    
        void setMaxNumOfCharacters(unsigned int max)
        {
            mMaxCharacters = max;
        }
    
        void onFocus()
        {
            mFocused = true;
            mTextChanged = false;
            mHighlightText = true;
            setCursorIndex(mText.size());
        }
    
        void onFocusLost()
        {
            mFocused = false;
            mHighlightText = false;
            if (mTextChanged){
                mTextChanged = false;
                ofxDatGuiInternalEvent e(ofxDatGuiEventType::INPUT_CHANGED, 0);
                internalEventCallback(e);
            }
        }
    
        void onKeyPressed(int key)
        {
            if (!keyIsValid(key)) return;
            if (mHighlightText) {
            // if key is printable or delete
                if ((key >= 32 && key <= 255) || key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
                    setText("");
                    setCursorIndex(0);
                }
            }
            if (key == OF_KEY_BACKSPACE){
            // delete character at cursor position //
                if (mCursorIndex > 0) {
                    setText(mText.substr(0, mCursorIndex - 1) + mText.substr(mCursorIndex));
                    setCursorIndex(mCursorIndex - 1);
                }
            } else if (key == OF_KEY_LEFT) {
                setCursorIndex(max( (int) mCursorIndex - 1, 0));
            } else if (key == OF_KEY_RIGHT) {
                setCursorIndex(min( mCursorIndex + 1, (unsigned int) mText.size()));
            } else {
            // insert character at cursor position //
                setText(mText.substr(0, mCursorIndex) + (char)key + mText.substr(mCursorIndex));
                setCursorIndex(mCursorIndex + 1);
            }
            mHighlightText = false;
        }
    
        void setCursorIndex(int index)
        {
           if (index == 0) {
               mCursorX = mFont->rect(mRendered.substr(0, index)).getLeft();
           } else if (index > 0) {
               mCursorX = mFont->rect(mRendered.substr(0, index)).getRight();
           // if we're at a space append the width the font's '1' character //
               if (mText.at(index - 1) == ' ') mCursorX += mFont->rect("1").width;
           }
            if (mType == ofxDatGuiInputType::COLORPICKER) mCursorX += mFont->rect("#").width;
            mCursorIndex = index;
        }
    
    protected:
    
        bool keyIsValid(int key)
        {
            if (key == OF_KEY_BACKSPACE || key == OF_KEY_LEFT || key == OF_KEY_RIGHT){
                return true;
            }   else if (mType == ofxDatGuiInputType::COLORPICKER){
            // limit string length to six hex characters //
                if (!mHighlightText && mText.size() == 6){
                    return false;
            // allow numbers 0-9 //
                }   else if (key>=48 && key<=57){
                    return true;
            // allow letters a-f & A-F //
                }   else if ((key>=97 && key<=102) || (key>=65 && key<=70)){
                    return true;
                }   else{
            // an invalid key was entered //
                    return false;
                }
            }   else if (mType == ofxDatGuiInputType::NUMERIC){
            // allow dash (-) or dot (.) //
                if (key==45 || key==46){
                    return true;
            // allow numbers 0-9 //
                }   else if (key>=48 && key<=57){
                    return true;
                }   else{
            // an invalid key was entered //
                    return false;
                }
            }   else if (mType == ofxDatGuiInputType::ALPHA_NUMERIC){
            // limit range to printable characters http://www.ascii-code.com //
                if (key >= 32 && key <= 255) {
                    return true;
                }   else {
            // an invalid key was entered //
                    return false;
                }
            }   else{
            // invalid textfield type //
                return false;
            }
        }
    
    private:
    
        string mText;
        string mRendered;
        bool mFocused;
        bool mTextChanged;
        bool mHighlightText;
        bool mUpperCaseText;
        float mCursorX;
        ofRectangle mTextRect;
        ofRectangle mInputRect;
        unsigned int mCursorIndex;
        unsigned int mMaxCharacters;
        unsigned int mHighlightPadding;
        struct{
            struct {
                ofColor text;
                ofColor background;
            } active;
            struct {
                ofColor text;
                ofColor background;
            } inactive;
            ofColor cursor;
            ofColor highlight;
        } color;
        ofxDatGuiInputType mType;
        shared_ptr<ofxSmartFont> mFont;

};

