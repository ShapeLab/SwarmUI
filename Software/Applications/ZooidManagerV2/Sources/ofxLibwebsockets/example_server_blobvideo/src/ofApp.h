#pragma once

#include "ofMain.h"

// To set this up (note - 2 onward are already done in this app!)
// 1 - Clone https://github.com/robotconscience/ofxTurboJpeg into addons/ofxTurboJpeg
// 2 - Add "Copy files build phase" in the 'targets->build phases menu'
// 3 - Add ofxTurboJpeg/libs/turbo-jpeg/lib/osx/libturbojpeg.dylib to the phase
// 4 - Make sure the dylib is copied to the "executables" directory!
#include "ofxTurboJpeg.h"
#include "ofxLibwebsockets.h"

#define NUM_MESSAGES 20 // how many past messages we want to keep

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        ofxLibwebsockets::Server server;
        
        bool bVideoSetup;
        ofVideoGrabber video;
        ofxTurboJpeg turbo;
    
        //queue of rec'd messages
        ofTrueTypeFont font;
        vector<string> messages;
    
        // websocket methods
        void onConnect( ofxLibwebsockets::Event& args );
        void onOpen( ofxLibwebsockets::Event& args );
        void onClose( ofxLibwebsockets::Event& args );
        void onIdle( ofxLibwebsockets::Event& args );
        void onMessage( ofxLibwebsockets::Event& args );
        void onBroadcast( ofxLibwebsockets::Event& args );
};
