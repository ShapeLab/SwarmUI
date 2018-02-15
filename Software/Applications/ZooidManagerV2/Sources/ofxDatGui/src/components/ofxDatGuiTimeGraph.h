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

class ofxDatGuiTimeGraph : public ofxDatGuiComponent {

    public:
    
        void setDrawMode(ofxDatGuiGraph gMode)
        {
            switch (gMode) {
				case ofxDatGuiGraph::LINES : mDrawFunc = &ofxDatGuiTimeGraph::drawLines;
                break;
				case ofxDatGuiGraph::FILLED : mDrawFunc = &ofxDatGuiTimeGraph::drawFilled;
                break;
				case ofxDatGuiGraph::POINTS : mDrawFunc = &ofxDatGuiTimeGraph::drawPoints;
                break;
				case ofxDatGuiGraph::OUTLINE : mDrawFunc = &ofxDatGuiTimeGraph::drawOutline;
                break;
            }
        }

    protected:
    
        ofxDatGuiTimeGraph(string label) : ofxDatGuiComponent(label)
        {
            mDrawFunc = &ofxDatGuiTimeGraph::drawFilled;
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        void setTheme(const ofxDatGuiTheme* theme)
        {
            setComponentStyle(theme);
            mStyle.height = theme->layout.graph.height;
            mStyle.stripe.color = theme->stripe.graph;
            mColor.lines = theme->color.graph.lines;
            mColor.fills = theme->color.graph.fills;
            mPointSize = theme->layout.graph.pointSize;
            mLineWeight = theme->layout.graph.lineWeight;
            setWidth(theme->layout.width, theme->layout.labelWidth);
        }
    
        void setWidth(int width, float labelWidth)
        {
            ofxDatGuiComponent::setWidth(width, labelWidth);
            mPlotterRect.x = mLabel.width;
            mPlotterRect.y = mStyle.padding;
            mPlotterRect.width = mStyle.width - mStyle.padding - mLabel.width;
            mPlotterRect.height = mStyle.height - (mStyle.padding*2);
        }
    
        void draw()
        {
            if (!mVisible) return;
            ofPushStyle();
                ofxDatGuiComponent::draw();
                ofSetColor(mStyle.color.inputArea);
                ofDrawRectangle(x + mPlotterRect.x, y + mPlotterRect.y, mPlotterRect.width, mPlotterRect.height);
                glColor3ub(mColor.fills.r, mColor.fills.g, mColor.fills.b);
                (*this.*mDrawFunc)();
            ofPopStyle();
        }
    
        void drawFilled()
        {
            float px = this->x + mPlotterRect.x;
            float py = this->y + mPlotterRect.y;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_TRIANGLE_STRIP);
            for (int i=0; i<pts.size(); i++) {
                glVertex2f(px+ pts[i].x, py + mPlotterRect.height);
                glVertex2f(px+ pts[i].x, py + pts[i].y);
            }
            glEnd();
        }

        void drawOutline()
        {
            float px = this->x + mPlotterRect.x;
            float py = this->y + mPlotterRect.y;
            glLineWidth(mLineWeight);
            glBegin(GL_LINE_LOOP);
            glVertex2f(px+mPlotterRect.width, py+mPlotterRect.height);
            for (int i=0; i<pts.size(); i++) glVertex2f(px+pts[i].x, py+pts[i].y);
            glVertex2f(px, py+mPlotterRect.height);
            glEnd();
        }
    
        void drawLines()
        {
            float px = this->x + mPlotterRect.x;
            float py = this->y + mPlotterRect.y;
            glLineWidth(mLineWeight);
            glBegin(GL_LINE_STRIP);
            for (int i=0; i<pts.size(); i++) glVertex2f(px+pts[i].x, py+pts[i].y);
            glEnd();
        }
    
        void drawPoints()
        {
            float px = this->x + mPlotterRect.x;
            float py = this->y + mPlotterRect.y;
            glPointSize(mLineWeight);
            glLineWidth(mLineWeight);
            glBegin(GL_POINTS);
            for (int i=0; i<pts.size(); i++) glVertex2f(px+pts[i].x, py+pts[i].y);
            glEnd();
        }
    
        void setPosition(int x, int y)
        {
            ofxDatGuiComponent::setPosition(x, y);
        }

        int mPointSize;
        int mLineWeight;
        struct{
            ofColor lines;
            ofColor fills;
        } mColor;
        vector<ofVec2f> pts;
        ofRectangle mPlotterRect;
        void (ofxDatGuiTimeGraph::*mDrawFunc)() = nullptr;
};

class ofxDatGuiWaveMonitor : public ofxDatGuiTimeGraph {

    public:

        ofxDatGuiWaveMonitor(string label, float frequency, float amplitude) : ofxDatGuiTimeGraph(label)
        {
            mFrequencyLimit = 100;
            setAmplitude(amplitude);
            setFrequency(frequency);
            mType = ofxDatGuiType::WAVE_MONITOR;
            setTheme(ofxDatGuiComponent::getTheme());
        }
    
        static ofxDatGuiWaveMonitor* getInstance()
        {
            return new ofxDatGuiWaveMonitor("X", 0, 0);
        }
    
    // amplitude is a multiplier that affect the vertical height of the wave and should be a value between 0 & 1 //
        void setAmplitude(float amp)
        {
            mAmplitude = amp;
            if (mAmplitude < 0) {
                mAmplitude = 0;
            }   else if (mAmplitude > MAX_AMPLITUDE){
                mAmplitude = MAX_AMPLITUDE;
            }
            graph();
        }
    
   // frequency is a percentage of the limit to ensure the value is always in range //
        void setFrequency(float freq)
        {
            mFrequency = (freq/mFrequencyLimit)*100.0f;
            graph();
        }
    
        void setFrequencyLimit(float limit)
        {
            mFrequencyLimit = limit;
            setFrequency(mFrequency);
        }
    
        void setTheme(const ofxDatGuiTheme* tmplt)
        {
            ofxDatGuiTimeGraph::setTheme(tmplt);
            graph();
        }
    
        void setWidth(int width, float labelWidth)
        {
            ofxDatGuiTimeGraph::setWidth(width, labelWidth);
            graph();
        }
    
        void graph()
        {
            pts.clear();
            float step = 1.0/mPlotterRect.width;
            float yAmp = (mPlotterRect.height/2) * (mAmplitude/float(MAX_AMPLITUDE));
            for (int i=mPlotterRect.width; i>0; i--) {
                float yp = mPlotterRect.height/2 + (sin((i*step)*(2*mFrequency)*PI) * yAmp);
                pts.push_back(ofVec2f(i, yp));
            }
        }
    
        void update(bool ignoreMouseEvents)
        {
            pts[0].y = pts[pts.size()-1].y;
            for (int i=mPlotterRect.width-1; i>0; i--) pts[i].y = pts[i-1].y;
        }
    
    private:
        float mAmplitude;
        float mFrequency;
        float mFrequencyLimit;
        static const int MAX_AMPLITUDE = 1;

};

class ofxDatGuiValuePlotter : public ofxDatGuiTimeGraph {

    public:

        ofxDatGuiValuePlotter(string label, float min, float max) : ofxDatGuiTimeGraph(label)
        {
            mSpeed = 5.0f;
            setRange(min, max);
            mType = ofxDatGuiType::VALUE_PLOTTER;
        }
    
        static ofxDatGuiValuePlotter* getInstance()
        {
            return new ofxDatGuiValuePlotter("X", 0, 0);
        }
    
        void setRange(float min, float max)
        {
            mMin = min;
            mMax = max;
            setValue((max+min)/2);
        }
    
        void setSpeed(float speed)
        {
            if (speed != mSpeed){
                pts.clear();
                mSpeed = speed;
            }
        }

        void setValue(float value)
        {
            mVal = value;
            if (mVal > mMax){
                mVal = mMax;
            }   else if (mVal < mMin){
                mVal = mMin;
            }
        }
    
        float getMin()
        {
            return mMin;
        }
    
        float getMax()
        {
            return mMax;
        }
    
        float getRange()
        {
            return mMax-mMin;
        }
    
        void update(bool ignoreMouseEvents)
        {
        // shift all points over before adding new value //
            for (int i=0; i<pts.size(); i++) pts[i].x -= mSpeed;
            int i = 0;
            while(i < pts.size())
            {
                if (pts.at(i).x <= 0) {
                    pts.erase(pts.end() - 1);
                } else if (pts.at(i).x <= mSpeed) {
                    pts.at(i).x = mLineWeight / 2;
                }
                i++;
            }
            float height = mPlotterRect.height - (mPlotterRect.height * ofxDatGuiScale(mVal, mMin, mMax));
            pts.insert(pts.begin(), ofVec2f(mPlotterRect.width, height));
        }
    
    private:
        float mVal;
        float mMin;
        float mMax;
        float mSpeed;
};




