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

class ofxDatGuiButton;
class ofxDatGuiToggle;
class ofxDatGuiSlider;
class ofxDatGuiDropdown;
class ofxDatGuiTextInput;
class ofxDatGui2dPad;
class ofxDatGuiColorPicker;
class ofxDatGuiMatrix;
class ofxDatGuiScrollView;
class ofxDatGuiScrollViewItem;

enum ofxDatGuiEventType
{
    GUI_TOGGLED = 0,
    BUTTON_CLICKED,
    BUTTON_TOGGLED,
    INPUT_CHANGED,
    COLOR_CHANGED,
    SLIDER_CHANGED,
    OPTION_SELECTED,
    GROUP_TOGGLED,
    VISIBILITY_CHANGED,
    MATRIX_BUTTON_TOGGLED
};

class ofxDatGuiInternalEvent{
    
    public:
        ofxDatGuiInternalEvent(int eType, int eIndex){
            type = eType;
            index = eIndex;
        };
        int type;
        int index;
};

class ofxDatGuiButtonEvent{

    public:
        ofxDatGuiButtonEvent(ofxDatGuiButton* t)
        {
            target = t;
        }
    ofxDatGuiButton* target;
};

class ofxDatGuiToggleEvent{

    public:
        ofxDatGuiToggleEvent(ofxDatGuiToggle* t, bool e = false)
        {
            target = t;
            checked = e;
        }
    bool checked;
    ofxDatGuiToggle* target;
};

class ofxDatGuiSliderEvent{

    public:
        ofxDatGuiSliderEvent(ofxDatGuiSlider* t, float v, float s)
        {
            value = v;
            scale = s;
            target = t;
        }
    float value;
    float scale;
    ofxDatGuiSlider* target;
};

class ofxDatGuiTextInputEvent{

    public:
        ofxDatGuiTextInputEvent(ofxDatGuiTextInput* t, std::string s)
        {
            text = s;
            target = t;
        }
    std::string text;
    ofxDatGuiTextInput* target;
};

class ofxDatGuiColorPickerEvent{
    
    public:
        ofxDatGuiColorPickerEvent(ofxDatGuiColorPicker* t, ofColor c)
        {
            color = c;
            target = t;
        }
    ofColor color;
    ofxDatGuiColorPicker* target;
};

class ofxDatGuiDropdownEvent{

    public:
        ofxDatGuiDropdownEvent(ofxDatGuiDropdown* t, int p, int c)
        {
            child = c;
            parent = p;
            target = t;
        }
    int child;
    int parent;
    ofxDatGuiDropdown* target;
};

class ofxDatGuiScrollViewEvent{

    public:
        ofxDatGuiScrollViewEvent(ofxDatGuiScrollView* p, ofxDatGuiScrollViewItem* b)
        {
            parent = p;
            target = b;
        }
    ofxDatGuiScrollView* parent;
    ofxDatGuiScrollViewItem* target;
};

class ofxDatGui2dPadEvent{

    public:
        ofxDatGui2dPadEvent(ofxDatGui2dPad* t, float xp, float yp)
        {
            x = xp;
            y = yp;
            target = t;
        }
    float x;
    float y;
    ofxDatGui2dPad* target;
};

class ofxDatGuiMatrixEvent{

    public:
        ofxDatGuiMatrixEvent(ofxDatGuiMatrix* t, int i, bool e)
        {
            child = i;
            target = t;
            enabled = e;
        }
    int child;
    bool enabled;
    ofxDatGuiMatrix* target;
};



