#pragma once

#include "ofMain.h"
#include <iostream>
#include <cmath>
#include "ZooidManager.h"
#include "ofxDatGui.h"

class GUILabels {
public:
    const string Header = "Settings";
    const string ManagerEnable = "ZooidManager ON";
    const string ControlMenuLabel = "Zooid Placement Optimization";
    const string ControlMenu = "Zooid Control Options";
    const string ControlOptPlanning = "Planning path";
    const string ControlOptDestination = "Final destination";
    
    const string OptimizationEnable = "Placement Optimization";
     string NumZooids = "Number of Zooids: ";
    const string NumZooidsMatrix = "Zooid Number Control Buttons";
    
    const string ServerLabel = "Network Communication";
    const string udpEnable = "Enable UDP";
    const string ipLabel = "Server IP";
    const string clientIpLabel = "Client IP";
    
    const string webEnable = "Enable Web";
    const string webPortlabel = "Web port";

};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

    void initGUI();
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void onTextInputEvent(ofxDatGuiTextInputEvent e);
    void onDropdownEvent(ofxDatGuiDropdownEvent e);
    void onMatrixEvent(ofxDatGuiMatrixEvent e);
    void onButtonEvent(ofxDatGuiButtonEvent e);


private:

	ZooidManager zooidManager;
    ofxDatGui* gui;
    ofxDatGuiLabel* numZooidLabel;
	ofTrueTypeFont myFont;

	int goalClicked;
	int robotToCommand;
    
    GUILabels guiLabels;
};
