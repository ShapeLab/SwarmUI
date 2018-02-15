#pragma once

// This example is a port of the GPU particle example, but with a twist:
// The position texture is streamed via ofxLibwebsockets to clients!

#include "ofMain.h"
#include "ofxLibwebsockets.h"

// Struct for doing PingPong quickly and easy
//
// Because on GPU you can«t write over the texture that you are reading we are
// using to pair of ofFbo attached together on what we call pingPongBuffer
// Learn more about Ping-Pong at:
//
// http://www.comp.nus.edu/~ashwinna/docs/PingPong_FBO.pdf
// http://www.seas.upenn.edu/~cis565/fbo.htm#setupgl4
//
struct pingPongBuffer {
public:
    void allocate( int _width, int _height, int _internalformat = GL_RGBA, float _dissipation = 1.0f){
        // Allocate
        for(int i = 0; i < 2; i++){
            FBOs[i].allocate(_width,_height, _internalformat );
            FBOs[i].getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        }
        
        // Clean
        clear();
        
        // Set everything to 0
        flag = 0;
        swap();
        flag = 0;
    }
    
    void swap(){
        src = &(FBOs[(flag)%2]);
        dst = &(FBOs[++(flag)%2]);
    }
    
    void clear(){
        for(int i = 0; i < 2; i++){
            FBOs[i].begin();
            ofClear(0,255);
            FBOs[i].end();
        }
    }
    
    ofFbo& operator[]( int n ){ return FBOs[n];}
    
    ofFbo   *src;       // Source       ->  Ping
    ofFbo   *dst;       // Destination  ->  Pong
private:
    ofFbo   FBOs[2];    // Real addresses of ping/pong FBO«s
    int     flag;       // Integer for making a quick swap
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofxLibwebsockets::Server server;
    
        // begin GPU Particle vars
        void setupGPUParticles();
        void updateGPUParticles();
    
        ofShader    updatePos;
        ofShader    updateVel;
        ofShader    updateRender;
        
        pingPongBuffer posPingPong;
        pingPongBuffer velPingPong;
        
        ofFbo   renderFBO;
        
        ofImage sparkImg;
        
        float   timeStep;
        float   particleSize;
        
        int     width, height;
        int     imgWidth, imgHeight;
        int     numParticles;
        int     textureRes;
        
        
        ofVboMesh mesh;
        // end GPU particle vars
};
