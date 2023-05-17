#pragma once

#include "ofMain.h"
#include <iostream>
#include <cmath>
#include "ZooidManager.h"
#include "ofxDatGui.h"
#include <exception>


class GUILabels {
public:
    const string Header = "Settings";
    const string ManagerEnable = "ZooidManager ON";
    const string ControlMenuLabel = "Zooid Placement Optimization";
    const string ControlMenu = "Zooid Control Options";
    const string ControlOptPlanning = "Planning path";
    const string ControlOptDestination = "Final destination";
    
    const string OptimizationEnable = "Placement Optimization";
    const string NumZooids = "Number of Zooids: ";
    const string NumZooidsMatrix = "Zooid Number Control Buttons";
    
    const string ServerLabel = "Network Communication";
    const string UdpEnable = "Enable UDP";
    const string IpLabel = "Server IP";
    const string ClientIpLabel = "Client IP";

    const string Dimensions = "World Dimensions (in m)";
    const string Width = "Width";
    const string Height= "Height";

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

    bool is_number(const std::string& s);
private:

	ZooidManager zooidManager;
    ofxDatGui* gui;
    ofxDatGuiLabel* numZooidLabel;
	ofTrueTypeFont myFont;

	int goalClicked;
	int robotToCommand;
    
    GUILabels guiLabels;
};
