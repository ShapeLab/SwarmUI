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
#include "ofxDatGuiGroups.h"
#include "ofxDatGuiControls.h"

class ofxDatGui : public ofxDatGuiInteractiveObject
{
    
    public:
    
        ofxDatGui(int x, int y);
        ofxDatGui(ofxDatGuiAnchor anchor = ofxDatGuiAnchor::TOP_LEFT);
        ~ofxDatGui();
    
        void draw();
        void update();
        void focus();
        void expand();
        void toggle();
        void collapse();
    
        void setWidth(int width, float labelWidth = 0.35f);
        void setVisible(bool visible);
        void setEnabled(bool enabled);
        void setOpacity(float opacity);
        void setPosition(int x, int y);
        void setPosition(ofxDatGuiAnchor anchor);
        void setTheme(ofxDatGuiTheme* t, bool applyImmediately = false);
        void setAutoDraw(bool autodraw, int priority = 0);
        void setLabelAlignment(ofxDatGuiAlignment align);
        static void setAssetPath(string path);
        static string getAssetPath();
    
        int getWidth();
        int getHeight();
        bool getFocused();
        bool getVisible();
        bool getAutoDraw();
        bool getMouseDown();
        ofPoint getPosition();
    
        ofxDatGuiHeader* addHeader(string label = "", bool draggable = true);
        ofxDatGuiFooter* addFooter();
        ofxDatGuiLabel* addLabel(string label);
        ofxDatGuiButton* addButton(string label);
        ofxDatGuiToggle* addToggle(string label, bool state = false);
        ofxDatGuiSlider* addSlider(string label, float min, float max);
        ofxDatGuiSlider* addSlider(string label, float min, float max, float val);
        ofxDatGuiSlider* addSlider(ofParameter<int> & p);
        ofxDatGuiSlider* addSlider(ofParameter<float> & p);
        ofxDatGuiTextInput* addTextInput(string label, string value = "");
        ofxDatGuiDropdown* addDropdown(string label, vector<string> options);
        ofxDatGuiFRM* addFRM(float refresh = 1.0f);
        ofxDatGuiBreak* addBreak();
        ofxDatGui2dPad* add2dPad(string label);
        ofxDatGui2dPad* add2dPad(string label, ofRectangle bounds);
        ofxDatGuiWaveMonitor* addWaveMonitor(string label, float min, float max);
        ofxDatGuiValuePlotter* addValuePlotter(string label, float min, float max);
        ofxDatGuiColorPicker* addColorPicker(string label, ofColor color = ofColor::black);
        ofxDatGuiMatrix* addMatrix(string label, int numButtons, bool showLabels = false);
        ofxDatGuiFolder* addFolder(string label, ofColor color = ofColor::white);
        ofxDatGuiFolder* addFolder(ofxDatGuiFolder* folder);
    
        ofxDatGuiHeader* getHeader();
        ofxDatGuiFooter* getFooter();
        ofxDatGuiLabel* getLabel(string label, string folder = "");
        ofxDatGuiButton* getButton(string label, string folder = "");
        ofxDatGuiToggle* getToggle(string label, string folder = "");
        ofxDatGuiSlider* getSlider(string label, string folder = "");
        ofxDatGui2dPad* get2dPad(string label, string folder = "");
        ofxDatGuiTextInput* getTextInput(string label, string folder = "");
        ofxDatGuiColorPicker* getColorPicker(string label, string folder = "");
        ofxDatGuiMatrix* getMatrix(string label, string folder = "");
        ofxDatGuiWaveMonitor* getWaveMonitor(string label, string folder = "");
        ofxDatGuiValuePlotter* getValuePlotter(string label, string folder = "");
        ofxDatGuiFolder* getFolder(string label);
        ofxDatGuiDropdown* getDropdown(string label);
    
    private:
    
        int mIndex;
        int mWidth;
        int mHeight;
        int mRowSpacing;
        float mAlpha;
        float mLabelWidth;
        bool mMoving;
        bool mVisible;
        bool mEnabled;
        bool mExpanded;
        bool mAutoDraw;
        bool mMouseDown;
        bool mAlphaChanged;
        bool mWidthChanged;
        bool mThemeChanged;
        bool mAlignmentChanged;
        ofColor mGuiBackground;
    
        ofPoint mPosition;
        ofRectangle mGuiBounds;
        ofxDatGuiAnchor mAnchor;
        ofxDatGuiHeader* mGuiHeader;
        ofxDatGuiFooter* mGuiFooter;
        ofxDatGuiTheme* mTheme;
        ofxDatGuiAlignment mAlignment;
        vector<ofxDatGuiComponent*> items;
        vector<ofxDatGuiComponent*> trash;
        static ofxDatGui* mActiveGui;
        static vector<ofxDatGui*> mGuis;
        static unique_ptr<ofxDatGuiTheme> theme;
    
        void init();
        void layoutGui();
        void anchorGui();
        void moveGui(ofPoint pt);
        bool hitTest(ofPoint pt);
        void attachItem(ofxDatGuiComponent* item);
    
        void onDraw(ofEventArgs &e);
        void onUpdate(ofEventArgs &e);
        void onWindowResized(ofResizeEventArgs &e);
    
        ofxDatGuiComponent* getComponent(string key);
        ofxDatGuiComponent* getComponent(ofxDatGuiType type, string label);
        void onInternalEventCallback(ofxDatGuiInternalEvent e);
        void onButtonEventCallback(ofxDatGuiButtonEvent e);
        void onToggleEventCallback(ofxDatGuiToggleEvent e);
        void onSliderEventCallback(ofxDatGuiSliderEvent e);
        void onTextInputEventCallback(ofxDatGuiTextInputEvent e);
        void onDropdownEventCallback(ofxDatGuiDropdownEvent e);
        void on2dPadEventCallback(ofxDatGui2dPadEvent e);
        void onColorPickerEventCallback(ofxDatGuiColorPickerEvent e);
        void onMatrixEventCallback(ofxDatGuiMatrixEvent e);

};
