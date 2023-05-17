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
    
        std::string file();
        int size();
        std::string name();
        void name(std::string name);
        void draw(std::string s, int x, int y);
    
        ofRectangle rect(std::string s, int x=0, int y=0);
        float width(std::string s, int x=0, int y=0);
        float height(std::string s, int x=0, int y=0);
        float getLineHeight();
    
    /*
        static methods
    */
        static std::shared_ptr<ofxSmartFont> add(std::string file, int size, std::string name = "");
        static std::shared_ptr<ofxSmartFont> get(std::string name);
        static std::shared_ptr<ofxSmartFont> get(std::string name, int size);
        static std::shared_ptr<ofxSmartFont> get(std::vector<std::string> keys, int size);
        static void list();
    
        static std::vector<std::shared_ptr<ofxSmartFont>> mFonts;
    
    private:
    
        ofxSmartFont(std::string file, int size, std::string name)
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
    
        static void log(std::string msg);

        int mSize;
        std::string mFile;
        std::string mName;
        ofTrueTypeFont ttf;
    
};

