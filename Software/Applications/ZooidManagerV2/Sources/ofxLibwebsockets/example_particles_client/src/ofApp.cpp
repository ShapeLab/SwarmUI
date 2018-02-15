#include "ofApp.h"

ofPixels sendPixels;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    // setup client @ port 9093
    client.connect("localhost", 9093);
    client.addListener(this);
    
    // setup GPU particle stuff
    setupGPUParticles();
}

//--------------------------------------------------------------
void ofApp::update(){
    // if we have some buffer data, load into our texture
    if ( buff.size() != 0 ){
        mutex.lock();
        
		int size = buff.size();

        float * incoming = new float[size ];
        memcpy(incoming, buff.getData(), buff.size());
        
        posFBO.getTexture().loadData(incoming, textureRes, textureRes, GL_RGB);
        mutex.unlock();
        updateGPUParticles();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofSetColor(100,255,255);
    renderFBO.draw(0,0);
    
    ofSetColor(255);
    ofDrawBitmapString("Fps: " + ofToString( ofGetFrameRate()), 15,15);
}


//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on connected";
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on open";
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on close";
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    ofLogVerbose()<<"on idle";
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
    // need to load this next frame!    buff.clear();
    mutex.lock();
    buff.set(args.data.getData(), args.data.size());
    mutex.unlock();
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}

//--------------------------------------------------------------
void ofApp::setupGPUParticles(){
    particleSize = 30.0f;
    numParticles = 1000;
    
    // Width and Heigth of the windows
    width = ofGetWindowWidth();
    height = ofGetWindowHeight();
    
    // Frag, Vert and Geo shaders for the rendering process of the spark image
#ifndef TARGET_WIN32
    updateRender.setGeometryInputType(GL_POINTS);
	updateRender.setGeometryOutputType(GL_TRIANGLE_STRIP);
	updateRender.setGeometryOutputCount(6);
    updateRender.load("shaders/render.vert","shaders/render.frag","shaders/render.geom");
#else
    updateRender.load("shaders/render.vert","shaders/render.frag");
#endif
    
    // Seting the textures where the information ( position and velocity ) will be
    textureRes = (int)sqrt((float)numParticles);
    numParticles = textureRes * textureRes;
    posFBO.allocate(textureRes, textureRes,GL_RGB32F);
    posFBO.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    // Loading and setings of the variables of the textures of the particles
    sparkImg.load("spark.png");
    imgWidth = sparkImg.getWidth();
    imgHeight = sparkImg.getHeight();
    
    // Allocate the final
    renderFBO.allocate(width, height, GL_RGB32F);
    renderFBO.begin();
    ofClear(0, 0, 0, 255);
    renderFBO.end();
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int x = 0; x < textureRes; x++){
        for(int y = 0; y < textureRes; y++){
            mesh.addVertex(ofVec3f(x,y));
            mesh.addTexCoord(ofVec2f(x, y));
        }
    }
}

//--------------------------------------------------------------
void ofApp::updateGPUParticles(){
    // Rendering
    renderFBO.begin();
    ofClear(0,0,0,0);
    updateRender.begin();
    updateRender.setUniformTexture("posTex", posFBO.getTexture(), 0);
    updateRender.setUniformTexture("sparkTex", sparkImg.getTexture() , 1);
    updateRender.setUniform1i("resolution", (float)textureRes);
    updateRender.setUniform2f("screen", (float)width, (float)height);
    updateRender.setUniform1f("size", (float)particleSize);
    updateRender.setUniform1f("imgWidth", (float)sparkImg.getWidth());
    updateRender.setUniform1f("imgHeight", (float)sparkImg.getHeight());
    
    ofPushStyle();
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    ofSetColor(255);
    
    mesh.draw();
    
    ofDisableBlendMode();
    glEnd();
    
    updateRender.end();
    renderFBO.end();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
