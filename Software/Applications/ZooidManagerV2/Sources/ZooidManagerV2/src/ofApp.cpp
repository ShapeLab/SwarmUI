#include "ofApp.h"
#define dimensionX                          0.8128f
#define dimensionY                          0.508f

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    
    goalClicked = -1;
    robotToCommand = -1;
    
    myFont.load("MerriweatherSans-Regular.ttf", 13);
    
    
    zooidManager.init();
    initGUI();
}

//--------------------------------------------------------------
void ofApp::update() {
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackgroundGradient(ofColor(220, 240, 240), ofColor(180, 210, 210));
    
    ofPushMatrix();
    {
        ofScale(ofGetWidth() / DIMENSION_X, ofGetHeight() / DIMENSION_Y);
        ofRotate(180.0f, 0.0, 0.0f, 1.0f);
        ofTranslate(-DIMENSION_X, -DIMENSION_Y);
        
        zooidManager.drawZooids();
    }
    ofPopMatrix();
    
    ofSetColor(0);
    
    // draw the robots' id
    for (int i = 0; i < zooidManager.getNbZooids(); i++) {
        string s = to_string(zooidManager.getZooid(i).getId());
        float x = ofGetWidth() - zooidManager.getZooid(i).getPosition().x / dimensionX * ofGetWidth() - myFont.stringWidth(s) / 2.0f;
        float y = ofGetHeight() - zooidManager.getZooid(i).getPosition().y / dimensionY * ofGetHeight() + myFont.stringHeight(s) / 2.0f;
        myFont.drawString(s, x, y);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
//    if(key == OF_KEY_LEFT){
//        for(int i=0;i<zooidManager.getNbZooids();++i){
//            zooidManager.rotateZooid(i, 5.0f);
//        }
//    }
//        
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    
    switch (key) {
            
        case ' ':
            switch (zooidManager.getSimulationMode()) {
                case On:
                    zooidManager.setSimulationMode(Off);
                    break;
                case Off:
                    zooidManager.setSimulationMode(NoPlanning);
                    break;
                case NoPlanning:
                    zooidManager.setSimulationMode(On);
                    break;
                default:
                    break;
            }
            
            break;
        case OF_KEY_RETURN:
            if (zooidManager.getAssignmentMode() == OptimalAssignment)
                zooidManager.setAssignmentMode(NaiveAssignment);
            else if (zooidManager.getAssignmentMode() == NaiveAssignment)
                zooidManager.setAssignmentMode(OptimalAssignment);
            
            zooidManager.assignRobots();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
    float mouseX = DIMENSION_X - (float) x / ofGetWidth() * DIMENSION_X;
    float mouseY = DIMENSION_Y - (float) y / ofGetHeight() * DIMENSION_Y;
    
    if (mouseX > DIMENSION_X) mouseX = DIMENSION_X;
    if (mouseX < 0.0f) mouseX = 0.0f;
    if (mouseY > DIMENSION_Y) mouseY = DIMENSION_Y;
    if (mouseY < 0.0f) mouseY = 0.0f;
    
    if (button == OF_MOUSE_BUTTON_1 || button == OF_MOUSE_BUTTON_2) {
        for (int i = 0; i < zooidManager.getNbZooids(); i++) {
            if (zooidManager.getZooid(i).getState() > 0) {
                zooidManager.moveZooid(i, mouseX, mouseY);
            }
        }
    }
    if (goalClicked >= 0 && button == OF_MOUSE_BUTTON_3) {
        zooidManager.moveGoal(goalClicked, mouseX, mouseY);
    }
    zooidManager.assignRobots();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
    float mouseX = DIMENSION_X - (float) x / ofGetWidth() * DIMENSION_X;
    float mouseY = DIMENSION_Y - (float) y / ofGetHeight() * DIMENSION_Y;
    
    if (mouseX > DIMENSION_X) mouseX = DIMENSION_X;
    if (mouseX < 0.001f) mouseX = 0.001f;
    if (mouseY > DIMENSION_Y) mouseY = DIMENSION_Y;
    if (mouseY < 0.001f) mouseY = 0.001f;
    
    
    if (button == OF_MOUSE_BUTTON_1 ||  button == OF_MOUSE_BUTTON_2) {
        for (int i = 0; i < zooidManager.getNbZooids(); i++) {
            float distance = (zooidManager.getZooid(i).getPosition() - ofVec2f(mouseX, mouseY)).lengthSquared();
            if (distance < zooidManager.getZooid(i).getRadius() * zooidManager.getZooid(i).getRadius()) {
                
                if(!ofGetKeyPressed(OF_KEY_SHIFT) && !ofGetKeyPressed(OF_KEY_ALT) && !ofGetKeyPressed(OF_KEY_CONTROL))
                    zooidManager.setZooidInteraction(i, true, false, false, false);
                else
                    zooidManager.setZooidInteraction(i, false, ofGetKeyPressed(OF_KEY_SHIFT) && button == OF_MOUSE_BUTTON_1,
                                                     !ofGetKeyPressed(OF_KEY_SHIFT) && button == OF_MOUSE_BUTTON_2,
                                                     ofGetKeyPressed(OF_KEY_SHIFT) && button == OF_MOUSE_BUTTON_2);
                break;
            } else
                zooidManager.setZooidInteraction(i, false, false, false, false);
        }
    } else if (button == OF_MOUSE_BUTTON_3) {
        for (int i = 0; i < zooidManager.getNbGoals(); i++) {
            float distance = (zooidManager.getGoal(i).getPosition() - ofVec2f(mouseX, mouseY)).lengthSquared();
            if (distance < robotRadius * robotRadius)
                goalClicked = i;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    for (int i = 0; i < zooidManager.getNbZooids(); i++) {
        zooidManager.setZooidInteraction(i, false, false, false, false);
    }
    goalClicked = -1;
    zooidManager.assignRobots();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    ofSetWindowShape(w, w / 1.6f);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}

//--------------------------------------------------------------
void ofApp::initGUI() {
    // instantiate the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT);
    gui->addHeader(guiLabels.Header);
    
    gui->addToggle(guiLabels.ManagerEnable, (zooidManager.getSimulationMode() == SimulationMode::On) || (zooidManager.getSimulationMode() == SimulationMode::NoPlanning));
    
    //optimization selection
    gui->addToggle(guiLabels.OptimizationEnable, zooidManager.getAssignmentMode() == OptimalAssignment);
    gui->addBreak(); gui->addBreak();
    
    //Manager control menu
    gui->addLabel(guiLabels.ControlMenuLabel);
    vector<string> opts = {guiLabels.ControlOptPlanning, guiLabels.ControlOptDestination};
    
    ofxDatGuiDropdown *controlMenu = gui->addDropdown(guiLabels.ControlMenu, opts);
    switch(zooidManager.getSimulationMode()){
        case SimulationMode::On:
            controlMenu->select(0);
            break;
        case SimulationMode::NoPlanning:
        case SimulationMode::Off:
            controlMenu->select(1);
            break;
        default:
            break;
    }
    gui->addBreak(); gui->addBreak();
    
    numZooidLabel = gui->addLabel(guiLabels.NumZooids + to_string(zooidManager.getNbZooids()));
    
    gui->addButton("-");
    gui->addButton("+");
    
    gui->addBreak(); gui->addBreak();
    
    gui->addLabel(guiLabels.ServerLabel);
    ofxDatGuiFolder* udpFolder = gui->addFolder("UDP ", ofColor::white);
    udpFolder->addToggle(guiLabels.udpEnable, (zooidManager.getServerType() == ServerType::UDP) || (zooidManager.getServerType() == ServerType::UDP_WEB));
    udpFolder->addLabel(guiLabels.ipLabel);
    udpFolder->addTextInput(guiLabels.clientIpLabel, zooidManager.getUDPIPaddress());
    
    gui->addBreak(); gui->addBreak();
    
    ofxDatGuiFolder* webFolder = gui->addFolder("Web ", ofColor::white);
    webFolder->addToggle(guiLabels.webEnable, (zooidManager.getServerType() == ServerType::WEB) || (zooidManager.getServerType() == ServerType::UDP_WEB));
    webFolder->addTextInput(guiLabels.webPortlabel, to_string(zooidManager.getWebServerPort()));
    
    gui->addBreak(); gui->addBreak();
    
    gui->addFooter();
    
    ofxDatGuiThemeSmoke *theme = new ofxDatGuiThemeSmoke();
    theme->font.ptr = ofxSmartFont::add("ofxbraitsch/fonts/Raleway-Regular.ttf", 10);
    
    gui->setTheme(theme);
    
    gui->setOpacity(0.5f);
    gui->collapse();
    
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    
}

//--------------------------------------------------------------
void ofApp::onToggleEvent(ofxDatGuiToggleEvent e) {
    if (e.target->is(guiLabels.ManagerEnable)){
        if(gui->getToggle(guiLabels.ManagerEnable)->getChecked())
            zooidManager.setSimulationMode(SimulationMode::On);
        else
            zooidManager.setSimulationMode(SimulationMode::Off);
    }
    
    if (e.target->is(guiLabels.udpEnable)){
        if(gui->getToggle(guiLabels.udpEnable)->getChecked()){
            zooidManager.setServerType(ServerType::UDP);
            gui->getToggle(guiLabels.webEnable)->setChecked(false);
        }
        else{
            zooidManager.setServerType(ServerType::NONE);
        }
    }
    if (e.target->is(guiLabels.webEnable)){
        if(gui->getToggle(guiLabels.webEnable)->getChecked()){
            zooidManager.setServerType(ServerType::WEB);
            gui->getToggle(guiLabels.udpEnable)->setChecked(false);
        }
        else{
            zooidManager.setServerType(ServerType::NONE);
        }
    }
    if (e.target->is(guiLabels.OptimizationEnable)){
        if(gui->getToggle(guiLabels.OptimizationEnable)->getChecked())
            zooidManager.setAssignmentMode(OptimalAssignment);
        else
            zooidManager.setAssignmentMode(NaiveAssignment);
    }
}

//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e) {
    cout << "onTextInputEvent: " << e.target->getLabel() << " " << e.target->getText() << endl;
}

//--------------------------------------------------------------
void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e) {
    if(e.target->is(guiLabels.ControlMenu)){
        if(e.target->getLabel() == guiLabels.ControlOptPlanning)
            zooidManager.setSimulationMode(On);
        else if(e.target->getLabel() == guiLabels.ControlOptDestination)
            zooidManager.setSimulationMode(NoPlanning);
    }
}

//--------------------------------------------------------------
void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e) {
    cout << "onMatrixEvent " << e.child << " : " << e.enabled << endl;
    cout << "onMatrixEvent " << e.target->getLabel() << " : " << e.target->getSelected().size() << endl;
}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    if(e.target->getLabel() == "+"){
        zooidManager.addZooid();
        numZooidLabel->setLabel(guiLabels.NumZooids + to_string(zooidManager.getNbZooids()));
    }
    else if(e.target->getLabel() == "-"){
        zooidManager.removeZooid();
        numZooidLabel->setLabel(guiLabels.NumZooids + to_string(zooidManager.getNbZooids()));
    }
}
