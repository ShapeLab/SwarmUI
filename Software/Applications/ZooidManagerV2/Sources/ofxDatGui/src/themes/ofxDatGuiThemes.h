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
#include "ofxDatGuiTheme.h"

/*
    Stock Themes
*/

class ofxDatGuiThemeSmoke : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeSmoke()
        {
            stripe.visible = false;
            color.label = hex(0xF8F3F0);
            color.guiBackground = hex(0x2C3137);
            color.background = hex(0x343B41);
            color.slider.fill = hex(0x60B9ED);
            color.slider.text = hex(0xFFFFFF);
            color.inputAreaBackground = hex(0x434A50);
            color.textInput.text = hex(0xFFFFFF);
            color.textInput.highlight = hex(0x434A50);
            color.textInput.backgroundOnActive = hex(0x2C3137);
            color.backgroundOnMouseOver = hex(0x434A50);
            color.backgroundOnMouseDown = hex(0x2C3137);
            color.matrix.hover.button = hex(0x60B9ED);
            color.matrix.selected.button = hex(0x2C3137);
            init();
        }
};

class ofxDatGuiThemeWireframe : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeWireframe()
        {
            stripe.visible = false;
            color.label = hex(0x6E6E6E);
            color.icons = hex(0x6E6E6E);
            color.background = hex(0xFCFAFD);
            color.guiBackground = hex(0xD8D8DB);
            color.inputAreaBackground = hex(0xE9E9E9);
            color.slider.fill = hex(0x6E6E6E);
            color.slider.text = hex(0x6E6E6E);
            color.textInput.text = hex(0x6E6E6E);
            color.textInput.highlight = hex(0xFCFAFD);
            color.colorPicker.border = hex(0xDFDDDF);
            color.textInput.backgroundOnActive = hex(0xD1D1D1);
            color.backgroundOnMouseOver = hex(0xECECEC);
            color.backgroundOnMouseDown = hex(0xDFDDDF);
            color.matrix.normal.button = hex(0xDFDDDF);
            color.matrix.hover.button = hex(0x9C9DA1);
            color.matrix.selected.button = hex(0x6E6E6E);
            color.pad2d.line = hex(0x6E6E6E);
            color.pad2d.ball = hex(0x6E6E6E);
            color.graph.fills = hex(0x6E6E6E);
            init();
        }
};

class ofxDatGuiThemeMidnight : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeMidnight()
        {
            stripe.visible = false;
            color.label = hex(0xffffff);
            color.background = hex(0x011726);
            color.guiBackground = hex(0x4C5B66);
            color.inputAreaBackground = hex(0x273946);
            color.slider.fill = hex(0x0A1E2E);
            color.slider.text = hex(0xffffff);
            color.textInput.text = hex(0xffffff);
            color.textInput.highlight = hex(0x596872);
            color.textInput.backgroundOnActive = hex(0x0A1E2E);
            color.backgroundOnMouseOver = hex(0x273946);
            color.backgroundOnMouseDown = hex(0x000000);
            color.matrix.hover.button = hex(0x596872);
            color.matrix.selected.button = hex(0x60B1CC);
            color.graph.fills = hex(0x596872);
            init();
        }
};

class ofxDatGuiThemeAqua : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeAqua()
        {
            stripe.visible = false;
            color.label = hex(0xF8F3F0);
            color.guiBackground = hex(0xF8F3F0);
            color.background = hex(0x445966);
            color.inputAreaBackground = hex(0x61717D);
            color.slider.fill = hex(0xF8F3F0);
            color.slider.text = hex(0xFFFFFF);
            color.textInput.text = hex(0xFFFFFF);
            color.textInput.highlight = hex(0x445966);
            color.textInput.backgroundOnActive = hex(0x334553);
            color.backgroundOnMouseOver = hex(0x61717D);
            color.backgroundOnMouseDown = hex(0x334553);
            color.matrix.hover.button = hex(0x55666F);
            color.matrix.selected.button = hex(0x334553);
            init();
        }
};

class ofxDatGuiThemeCharcoal : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeCharcoal()
        {
            stripe.visible = false;
            color.label = hex(0x9C9DA1);
            color.icons = hex(0x9C9DA1);
            color.background = hex(0x28292E);
            color.guiBackground = hex(0x1E1F24);
            color.inputAreaBackground = hex(0x42424A);
            color.slider.fill = hex(0xF4BF39);
            color.slider.text = hex(0x9C9DA1);
            color.textInput.text = hex(0x9C9DA1);
            color.textInput.highlight = hex(0x28292E);
            color.colorPicker.border = hex(0xEEEEEE);
            color.textInput.backgroundOnActive = hex(0x1D1E22);
            color.backgroundOnMouseOver = hex(0x42424A);
            color.backgroundOnMouseDown = hex(0x1D1E22);
            color.matrix.hover.button = hex(0x9C9DA1);
            color.graph.fills = hex(0x9C9DA1);
            init();
        }
};

class ofxDatGuiThemeAutumn : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeAutumn()
        {
            stripe.visible = false;
            color.label = hex(0xF8F3F0);
            color.guiBackground = hex(0x7d7066);
            color.background = hex(0x4C4743);
            color.inputAreaBackground = hex(0xB5BCB2);
            color.slider.fill = hex(0xFFB230);
            color.slider.text = hex(0xF8F3F0);
            color.textInput.text = hex(0xF8F3F0);
            color.textInput.highlight = hex(0x4C4743);
            color.textInput.backgroundOnActive = hex(0x7d7066);
            color.backgroundOnMouseOver = hex(0x7d7066);
            color.backgroundOnMouseDown = hex(0x333333);
            color.matrix.hover.button = hex(0xC3A279);
            color.matrix.selected.button = hex(0x7d7066);
            init();
        }
};

class ofxDatGuiThemeCandy : public ofxDatGuiTheme{

    public:
    
        ofxDatGuiThemeCandy()
        {
            stripe.visible = false;
            color.label = hex(0xFFFFFF);
            color.icons = hex(0xFFFFFF);
            color.background = hex(0xFF4081);
            color.guiBackground = hex(0xEEEEEE);
            color.inputAreaBackground = hex(0xFF80AB);
            color.slider.fill = hex(0xF50057);
            color.slider.text = hex(0xFFFFFF);
            color.textInput.text = hex(0xFFFFFF);
            color.textInput.highlight = hex(0xFF4081);
            color.colorPicker.border = hex(0xDFDDDF);
            color.textInput.backgroundOnActive = hex(0xF50057);
            color.backgroundOnMouseOver = hex(0xFF80AB);
            color.backgroundOnMouseDown = hex(0xF50057);
            color.matrix.normal.button = hex(0xFFFFFF);
            color.matrix.normal.label = hex(0xFF4081);
            color.matrix.hover.button = hex(0xFF4081);
            color.matrix.selected.button = hex(0xF50057);
            color.pad2d.line = hex(0xFFFFFF);
            color.pad2d.ball = hex(0xFFFFFF);
            color.graph.fills = hex(0xF50057);
            init();
        }
};



