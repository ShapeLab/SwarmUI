#include "ofApp.h"

bool displayHelp = false;
float finalOrientation = 90.0f;

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);

    goalClicked = -1;
    robotToCommand = -1;

    myFont.load("MerriweatherSans-Regular.ttf", 13);
    removeZooidButton.set("-", ofVec2f(180.0f, ofGetHeight()- 112.0f), 20.0f);
    addZooidButton.set("+", ofVec2f(240.0f, ofGetHeight()- 112.0f), 20.0f);
    
    zooidManager.init();
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackgroundGradient(ofColor(220, 240, 240), ofColor(180, 210, 210));

    ofPushMatrix();
    {
        ofScale(ofGetWidth() / dimensionX, ofGetHeight() / dimensionY);
        ofRotate(180.0f, 0.0, 0.0f, 1.0f);
        ofTranslate(-dimensionX, -dimensionY);

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
    
    if (displayHelp) {
        ostringstream instructions;
        instructions << "Left click to move the zooid" << endl <<
                     "Right click to move the goal" << endl <<
                     "Press Space to turn ON/OFF the simulation" << endl <<
                     "Press Enter to toggle the goal assignment mode";
        ofSetColor(158, 173, 173, 85);
        float x = 10.0f;
        float y = ofGetHeight() - 25.0f - myFont.stringHeight(instructions.str());
        ofDrawRectRounded(x, y, 15.0f + myFont.stringWidth(instructions.str()),
                          myFont.stringHeight(instructions.str()) + 15.0f, 5.0f);
        ofSetColor(0);
        myFont.drawString(instructions.str(), x + 5.0f, y + 22.5f);
    } else {
        ostringstream docstring;
        docstring <<
                  "Number of Zooids:        " << zooidManager.getNbZooids() << endl <<
                  "Optimal positioning " << ((zooidManager.getAssignmentMode() == 1) ? "ON" : "OFF") << endl <<
                  "Refresh rates: " <<  nearbyint(zooidManager.getUpdateFrequency()) << "Hz - " << nearbyint(ofGetFrameRate()) << "fps" << endl <<
                  "Press ? for help" << endl;
        
        ofSetColor(158, 173, 173, 85);
        float x = 10.0f;
        float y = ofGetHeight();
        y -= 117.0f;//-25.0f - myFont.stringHeight(docstring.str());
        float w = 260.0f;//myFont.stringWidth(docstring.str()) + 15.0f;
        float h = 100.0f;//myFont.stringHeight(docstring.str()) + 15.0f;
        ofDrawRectRounded(x, y, w, h, 5.0f);


        ofSetColor(138, 153, 153, 100);

        addZooidButton.draw(ofVec2f(240.0f, ofGetHeight()- 112.0f));
        removeZooidButton.draw(ofVec2f(180.0f, ofGetHeight()- 112.0f));
        
        ofSetColor(0);
        myFont.drawString(docstring.str(), x + 5.0f, y + 22.5f);
    }

    ofFill();
    
    float ratio = (float)zooidManager.getNbConnectedReceivers()/(float)zooidManager.getNbRequiredReceivers();
    ofSetColor((int)(255.0f * (1.0f - ratio)), (int)(255.0f * ratio), 0);
    ofDrawCircle(30.0f, 30.0f, 25.0f);

    switch (zooidManager.getSimulationMode()) {
        case On:
            ofSetColor(ofColor::green);
            break;
        case Off:
            ofSetColor(ofColor::red);
            break;
        case NoPlanning:
            ofSetColor(ofColor::orange);
            break;
        default:
            break;
    }

    ofDrawCircle(30.0f, 30.0f, 20.0f);

    ofNoFill();
    ofSetColor(0);
    ofSetLineWidth(2.0f);
    ofDrawCircle(30.0f, 30.0f, 20.0f);
    ofSetLineWidth(1.0f);
    ofDrawCircle(30.0f, 30.0f, 25.0f);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    switch (key) {

        case '?':
        case '/':
            displayHelp = true;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

    switch (key) {
        case '?':
        case '/':
            displayHelp = false;
            break;
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

    float mouseX = dimensionX - (float) x / ofGetWidth() * dimensionX;
    float mouseY = dimensionY - (float) y / ofGetHeight() * dimensionY;

    if (mouseX > dimensionX) mouseX = dimensionX;
    if (mouseX < 0.0f) mouseX = 0.0f;
    if (mouseY > dimensionY) mouseY = dimensionY;
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

    float mouseX = dimensionX - (float) x / ofGetWidth() * dimensionX;
    float mouseY = dimensionY - (float) y / ofGetHeight() * dimensionY;

    if (mouseX > dimensionX) mouseX = dimensionX;
    if (mouseX < 0.001f) mouseX = 0.001f;
    if (mouseY > dimensionY) mouseY = dimensionY;
    if (mouseY < 0.001f) mouseY = 0.001f;

    
    if (button == OF_MOUSE_BUTTON_1 ||  button == OF_MOUSE_BUTTON_2) {
        if(addZooidButton.isColliding(ofVec2f(x,y))){
            addZooidButton.setClicked(true);
            zooidManager.addZooid();
        } else
            addZooidButton.setClicked(false);
        
        if(removeZooidButton.isColliding(ofVec2f(x,y))) {
            removeZooidButton.setClicked(true);
            zooidManager.removeZooid();
        } else removeZooidButton.setClicked(false);
        
        for (int i = 0; i < zooidManager.getNbZooids(); i++) {
            float distance = (zooidManager.getZooid(i).getPosition() - ofVec2f(mouseX, mouseY)).lengthSquared();
            if (distance < zooidManager.getZooid(i).getRadius() * zooidManager.getZooid(i).getRadius() && (button == OF_MOUSE_BUTTON_1 || button == OF_MOUSE_BUTTON_2)) {

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
    addZooidButton.setClicked(false);
    removeZooidButton.setClicked(false);
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
