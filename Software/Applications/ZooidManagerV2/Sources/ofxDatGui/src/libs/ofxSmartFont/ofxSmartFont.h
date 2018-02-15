/*
    Copyright (C) 2016 Stephen Braitsch [http://braitsch.io]
    ofxSmartFont v1.0.0 https://github.com/braitsch/ofxSmartFont

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
#include "ofTrueTypeFont.h"

class ofxSmartFont {

    public:
    
    /*
        instance methods
    */
    
        string file();
        int size();
        string name();
        void name(string name);
        void draw(string s, int x, int y);
    
        ofRectangle rect(string s, int x=0, int y=0);
        float width(string s, int x=0, int y=0);
        float height(string s, int x=0, int y=0);
        float getLineHeight();
    
    /*
        static methods
    */
        static shared_ptr<ofxSmartFont> add(string file, int size, string name = "");
        static shared_ptr<ofxSmartFont> get(string name);
        static shared_ptr<ofxSmartFont> get(string name, int size);
        static shared_ptr<ofxSmartFont> get(vector<string> keys, int size);
        static void list();
    
        static vector<shared_ptr<ofxSmartFont>> mFonts;
    
    private:
    
        ofxSmartFont(string file, int size, string name)
        {
            mSize = size;
            mFile = file;
            if (name != ""){
                mName = name;
            }   else {
                mName = file.substr(file.find_last_of("/\\") + 1);
            }
            if (ttf.load(mFile, mSize) == false){
                log("ERROR!! file : " + mFile + " NOT FOUND");
            }   else{
                log("new font added : " +mName+" @ pt size "+std::to_string(mSize));
            }
        }
    
        static void log(string msg);

        int mSize;
        string mFile;
        string mName;
        ofTrueTypeFont ttf;
    
};

