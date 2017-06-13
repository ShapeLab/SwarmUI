#include "ofApp.h"

int assignmentMode = 0;
bool displayHelp = false;
float finalOrientation = 90.0f;
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    goalClicked = -1;
    robotToCommand = -1;
    
    previousSimulationTimestamp = 0;
	simulationMode = NO_PLANNING;
    
    kSpeed = 0.4f;
    prefSpeed = kSpeed * maxSpeed;
    uncertaintyOffset = 0.05f * prefSpeed;
    
    robotArtwork.load("robot.svg");
    batteryArtwork.load("battery.svg");
    myFont.load("MerriweatherSans-Regular.ttf", 13);
    
    //create the socket and set to send to 127.0.0.1:11999
    udpSender.Create();
    udpSender.Connect("127.0.0.1",11999);
    udpSender.SetNonBlocking(true);
    
    udpReceiver.Create();
    udpReceiver.Bind(11998);
    udpReceiver.SetNonBlocking(true);
    
#ifdef TARGET_WIN32
	swarmReceivers.push_back(new SwarmReceiver("COM3"));
//	swarmReceivers.push_back(new SwarmReceiver("COM7"));
#else
	swarmReceivers.push_back(new SwarmReceiver(1));
	//    swarmReceivers.push_back(new SwarmReceiver(2));
#endif
    
	receiverConnected = true;
	for (int i = 0; i < swarmReceivers.size(); i++)
		receiverConnected &= swarmReceivers[i]->isInitialized();

	simulator.setTimeStep(timeStep);
    simulator.setAgentDefaults(neighborDist, maxNeighbors, robotRadius, goalRadius, prefSpeed, maxSpeed,
                               timeToOrientation, wheelTrack, uncertaintyOffset, maxAccel, hrvo::Vector2(0.0f, 0.0f), 0.0f);
    
    for (int i = 0; i < swarmReceivers.size() * NB_ROBOTS_PER_RECEIVER; i++)
    {
        float tmpAngle;
        ofVec2f finalPosition = ofVec2f(ofRandom(0.1f, dimensionX - 0.1f), ofRandom(0.1f, dimensionY - 0.1f));
        ofVec2f initialPosition = ofVec2f(ofRandom(0.1f, dimensionX-0.1f), ofRandom(0.1f, dimensionY - 0.1f));
        tmpAngle = ofRandom(-180.0f, 180.0f);
        
        ofColor tmpColor =  ofColor(ofRandomf()*255.0f, ofRandomf()*255.0f, ofRandomf()*255.0f);
        
        RobotState tmpRobot = RobotState(0, robotRadius, initialPosition, ofVec2f(0.0f, 0.0f), tmpAngle, tmpColor, 100);
        simulator.addGoal(hrvo::Vector2(finalPosition.x, finalPosition.y));
        unsigned int tmpId = simulator.addAgent(hrvo::Vector2(initialPosition.x, initialPosition.y),0);
        simulator.setAgentOrientation(tmpId, tmpAngle*PI/180.0f);
        tmpRobot.setId(tmpId);
        robotCollection.push_back(tmpRobot);
        robotUpdated.push_back(true);
    }
    assignGoalIndex(assignmentMode);
    previousSimulationTimestamp = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update()
{
    receiveAppGoals();
    readRobotsPositions();
	if(simulationMode == ON)
		runSimulation();
    sendRobotsOrders();
    sendPositions();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(220, 240, 240), ofColor(180, 210, 210));
	switch (simulationMode)
	{
	case ON:
		ofSetColor(ofColor::green);
		break;
	case OFF:
		ofSetColor(ofColor::red);
		break;
	case NO_PLANNING:
		ofSetColor(ofColor::orange);
		break;
	default:
		break;
	}

    ofFill();
    ofDrawCircle(30.0f, 30.0f, 20.0f);
    ofNoFill();
    ofSetColor(0);
    ofSetLineWidth(1.0f);
    ofDrawCircle(30.0f, 30.0f, 20.0f);
    
    ofPushMatrix();
    ofScale(ofGetWidth() / dimensionX, ofGetHeight() / dimensionY);
    ofRotate(180.0f, 0.0, 0.0f, 1.0f);
    ofTranslate(-dimensionX, -dimensionY);
    
    ofFill();
    ofSetColor(ofColor::darkGrey, 95);
    
	if (simulationMode == ON)
	{
		for (int i = 0; i < simulator.getNumAgents(); i++)
			ofDrawCircle(simulator.getAgentPosition(i).getX(), simulator.getAgentPosition(i).getY(), robotRadius);
	}

    for (int i = 0; i < robotCollection.size(); i++)
    {
        robotCollection[i].drawGoal();
        robotCollection[i].drawRobot(&robotArtwork, 1.0f);
        robotCollection[i].drawBatteryLevel(&batteryArtwork, 1.0f);
    }
    ofPopMatrix();
    
    ofSetColor(0);
    
    for (int i = 0; i < robotCollection.size(); i++)
    {
        string s = to_string(robotCollection[i].getId());
        float x = ofGetWidth() - robotCollection[i].getPosition().x/dimensionX*ofGetWidth() - myFont.stringWidth(s)/2.0f;
        float y = ofGetHeight() - robotCollection[i].getPosition().y/dimensionY*ofGetHeight() + myFont.stringHeight(s)/2.0f;
        myFont.drawString(s, x, y);
    }
    
    
    if(displayHelp)
    {
        ostringstream instructions;
        instructions << "Left click to move the robot" << endl <<
        "Right click to move the goal" << endl <<
        "Press Up/Down to change the maximum speed" << endl <<
        "Press Space to turn ON/OFF the simulation" << endl <<
        "Press Enter to toggle the goal assignement mode" << endl <<
        "Press 0-9 to select a robot and W-S-A-D to direct it";
        ofSetColor(158, 173, 173, 85);
        float x = 10.0f;
        float y = ofGetHeight() - 25.0f - myFont.stringHeight(instructions.str());
        ofDrawRectRounded(x, y, 15.0f + myFont.stringWidth(instructions.str()), myFont.stringHeight(instructions.str()) + 15.0f, 5.0f);
        ofSetColor(0);
        myFont.drawString(instructions.str(), x + 5.0f, y + 22.5f );
    }
    else
    {
        ostringstream docstring;
        docstring << "Press ? for help" << endl <<
        "kSpeed = " << kSpeed << endl <<
        "Optimal positioning " << ((assignmentMode==1) ? "ON" : "OFF") << endl <<
        "Framerate: " << (int)ofGetFrameRate() << endl;
        ofSetColor(158, 173, 173, 85);
        float x = 10.0f;
        float y = ofGetHeight();
        y-= 107.0f;//-25.0f - myFont.stringHeight(docstring.str());
        float w = 216;//myFont.stringWidth(docstring.str()) + 15.0f;
        float h = 100;//myFont.stringHeight(docstring.str()) + 15.0f;
        ofDrawRectRounded(x, y, w, h, 5.0f);
        ofSetColor(0);
        
        myFont.drawString( docstring.str(),x + 5.0f, y + 22.5f);
    }
}

//--------------------------------------------------------------
bool ofApp::runSimulation()
{
    uint64_t currentTimestep = ofGetElapsedTimeMillis();
    //running one step of the simmulation to compute new directions and motors values
    //has to run at the given frequency
    
    if (simulationMode == ON && (currentTimestep - previousSimulationTimestamp) >= timeStep * 500)
    {
        if(receiverConnected)
        {
            for (int i = 0; i < robotCollection.size(); i++)
            {
                //only if the simulated position is too far away
                //first update the simulation with the latest robots' positions and orientations
                hrvo::Vector2 simulatedPosition = simulator.getAgentPosition(robotCollection[i].getId());
                float distance = (robotCollection[i].getPosition() - ofVec2f(simulatedPosition.getX(),simulatedPosition.getY())).length();
                
                //if(distance > 0.1f)
                //{
                //    simulator.setAgentPosition(robotCollection[i].getId(), hrvo::Vector2(robotCollection[i].getPosition().x, robotCollection[i].getPosition().y));
                //    simulator.setAgentOrientation(robotCollection[i].getId(), robotCollection[i].getAngle()*PI / 180.0f);
                //}
            }
        }
        
        //then do the HRVO magic
        simulator.doStep();
        
        for (int i = 0; i < robotCollection.size(); i++)
        {
			//trick to slow them down around th goal, figure out something betteR
			hrvo::Vector2 distance = simulator.getAgentPosition(robotCollection[i].getId()) - simulator.getGoalPosition(simulator.getAgentGoal(robotCollection[i].getId()));
			float k =  pow(abs(distance), 2.0f)*500.0f;
			if (k >= 1.0f) k = 1.0f;
			simulator.setAgentPrefSpeed(i, prefSpeed * k);
			simulator.setAgentMaxSpeed(i, 1.1f * prefSpeed * k);

			if (simulator.getAgentReachedGoal(i))
				simulator.setAgentPrefSpeed(i, 0.0f);
			else
				simulator.setAgentPrefSpeed(i, prefSpeed * k);

            if (robotCollection[i].getTouch() == 0)
            {
                //if the swarmreicever is not connected, just use the simulation positions
                if(!receiverConnected)
                {
                    robotCollection[i].setPosition(simulator.getAgentPosition(robotCollection[i].getId()).getX(), simulator.getAgentPosition(robotCollection[i].getId()).getY());
                    robotCollection[i].setAngle(simulator.getAgentOrientation(robotCollection[i].getId()) * 180.0f / PI);
					robotCollection[i].setGoalReached(simulator.getAgentReachedGoal(robotCollection[i].getId()));
                }
				robotCollection[i].setGoalReached(simulator.getAgentReachedGoal(robotCollection[i].getId()));


            }
        }
        previousSimulationTimestamp = currentTimestep;
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void ofApp::sendRobotsOrders()
{
    if(receiverConnected)
    {
        for (int i = 0; i < robotCollection.size(); i++)
        {
			if (robotCollection[i].getTouch() == 0)
			{
				if (simulationMode == ON)
				{
					///////////////////////////////////////////////////////////////////
					//
					//  DONT FORGET !!!!!!!!
					///////////////////////////////////////////////////////////////////

					controlRobotPosition(robotCollection[i].getId(),
						simulator.getAgentPosition(robotCollection[i].getId()).getX(),
						simulator.getAgentPosition(robotCollection[i].getId()).getY(),
						robotCollection[i].getColor(),
						robotCollection[i].getAngle(),//finalOrientation,
						kSpeed*100.0f,// ofMap(abs(simulator.getAgentVelocity(robotCollection[i].getId())), 0.0f, prefSpeed, 0, 40.0f)-5.0f,
						robotCollection[i].isGoalReached());
					//controlRobotPosition(10+robotCollection[i].getId(),
					//	simulator.getAgentPosition(robotCollection[i].getId()).getX(),
					//	simulator.getAgentPosition(robotCollection[i].getId()).getY(),
					//	robotCollection[i].getColor(),
					//	finalOrientation,
					//	kSpeed*100.0f,// ofMap(abs(simulator.getAgentVelocity(robotCollection[i].getId())), 0.0f, prefSpeed, 0, 40.0f)-5.0f,
					//	robotCollection[i].isGoalReached());
				}
				else if (simulationMode == NO_PLANNING)
				{		
					///////////////////////////////////////////////////////////////////
					//
					//  DONT FORGET !!!!!!!!
					///////////////////////////////////////////////////////////////////

					controlRobotPosition(robotCollection[i].getId(),
						simulator.getGoalPosition(simulator.getAgentGoal(robotCollection[i].getId())).getX(),
						simulator.getGoalPosition(simulator.getAgentGoal(robotCollection[i].getId())).getY(),
						robotCollection[i].getColor(),
						robotCollection[i].getAngle(), //finalOrientation,
						kSpeed*100.0f,// ofMap(abs(simulator.getAgentVelocity(robotCollection[i].getId())), 0.0f, prefSpeed, 0, 40.0f)-5.0f,
						robotCollection[i].isGoalReached());
					//controlRobotPosition(10+robotCollection[i].getId(),
					//	simulator.getGoalPosition(simulator.getAgentGoal(robotCollection[i].getId())).getX(),
					//	simulator.getGoalPosition(simulator.getAgentGoal(robotCollection[i].getId())).getY(),
					//	robotCollection[i].getColor(),
					//	finalOrientation,
					//	kSpeed*100.0f,// ofMap(abs(simulator.getAgentVelocity(robotCollection[i].getId())), 0.0f, prefSpeed, 0, 40.0f)-5.0f,
					//	robotCollection[i].isGoalReached());
				}
			}
		}
    }
}

//--------------------------------------------------------------
void ofApp::readRobotsPositions()
{
	//updating the positions with the data received from robots
	for (int i = 0; i < swarmReceivers.size(); i++)
	{
		while (swarmReceivers[i]->availableMessages() > 0)
		{
			Message msg = swarmReceivers[i]->getLastMessage();
			uint16_t robotId = msg.getSenderId();

			if (robotId < robotCollection.size() && msg.getType() == TYPE_STATUS)
			{
				StatusMessage status;

				memcpy(&status, msg.getPayload(), sizeof(StatusMessage));

				float robotA = (float)(status.orientation) / 100.0f;
				float robotX = ofMap((float)status.positionX, coordinatesMinX, coordinatesMaxX, 0.0f, dimensionX);
				float robotY = ofMap((float)status.positionY, coordinatesMinY, coordinatesMaxY, 0.0f, dimensionY);

				robotCollection[robotId].setTouch(status.touch);
				robotCollection[robotId].setPosition(robotX, robotY);
				/*robotCollection[robotId].setAngle(robotA);*/
				robotCollection[robotId].setBatteryLevel(status.batteryLevel);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::controlRobotSpeed(int id, int8_t motor1, int8_t motor2, ofColor color)
{
    uint8_t buffer[5] = { 0 };
    
    if (id >= 0 && id < robotCollection.size())
    {
        buffer[0] = (motor1 <= 100) ? motor1 : 100;
        buffer[1] = (motor2 <= 100) ? motor2 : 100;
        buffer[2] = color.r;
        buffer[3] = color.g;
        buffer[4] = color.b;
		swarmReceivers[id/NB_ROBOTS_PER_RECEIVER]->sendUSB(TYPE_MOTORS_VELOCITY, robotCollection[id].getId(), sizeof(buffer), (uint8_t*)&buffer[0]);
    }
}

//--------------------------------------------------------------
void ofApp::controlRobotPosition(uint8_t id, float x, float y, ofColor color, float orientation, float preferredSpeed, bool isFinalGoal)
{
	PositionControlMessage msg;
    //if (id >= 0 && id < robotCollection.size())
    {
		float tmpX = x, tmpY = y;
		if (tmpX > dimensionX) tmpX = dimensionX;
		if (tmpX < 0.0f) tmpX = 0.0f;
		if (tmpY > dimensionY) tmpY = dimensionY;
		if (tmpY < 0.0f) tmpX = 0.0f;

		msg.positionX = (uint16_t)ofMap(tmpX, 0.0f, dimensionX, coordinatesMinX, coordinatesMaxX);
		msg.positionY = (uint16_t)ofMap(tmpY, 0.0f, dimensionY, coordinatesMinY, coordinatesMaxY);
		msg.colorRed = color.r;
		msg.colorGreen = color.g;
		msg.colorBlue = color.b;
		msg.orientation = (uint16_t)(orientation*100.0f);
		msg.preferredSpeed = (uint8_t)preferredSpeed;
		msg.isFinalGoal = isFinalGoal;
		msg.empty = 0xff;
		///////////////////////////////////////////////////////////////////
		//
		//  DONT FORGET !!!!!!!!
		///////////////////////////////////////////////////////////////////
		int receiverId = id / NB_ROBOTS_PER_RECEIVER;
		//int receiverId = id / NB_ROBOTS_PER_RECEIVER-1;
        swarmReceivers[receiverId]->sendUSB(TYPE_ROBOT_POSITION, id, sizeof(msg), (uint8_t*)&msg);
    }
}

//--------------------------------------------------------------
void ofApp::receiveAppGoals()
{
    char udpMessage[10000] = { 0 };
    
    while (udpReceiver.PeekReceive()>0)
    {
        int nbBytesReceived = udpReceiver.Receive(udpMessage, 10000);
		if (nbBytesReceived > 0)
		{
			for (int j = 0; j < nbBytesReceived / sizeof(RobotState); j++)
			{
				RobotState tmpRobot;
				memcpy(&tmpRobot, &udpMessage[j*sizeof(RobotState)], sizeof(RobotState));


				if (tmpRobot.getGoal().x < robotDiameter) tmpRobot.setGoal(robotDiameter, tmpRobot.getGoal().y);
				if (tmpRobot.getGoal().x > dimensionX - robotDiameter) tmpRobot.setGoal(dimensionX - robotDiameter, tmpRobot.getGoal().y);
				if (tmpRobot.getGoal().y < robotDiameter) tmpRobot.setGoal(tmpRobot.getGoal().x, robotDiameter);
				if (tmpRobot.getGoal().y > dimensionY - robotDiameter) tmpRobot.setGoal(tmpRobot.getGoal().x, dimensionY - robotDiameter);
				for (int i = 0; i < robotCollection.size(); i++)
				{
					unsigned int tmpId = tmpRobot.getId();
					auto it = find_if(robotCollection.begin(), robotCollection.end(), [&tmpId](RobotState& r) {return r.getId() == tmpId; });
					if (it != robotCollection.end())
					{
						    it->setAngle(tmpRobot.getAngle());
						it->setColor(tmpRobot.getColor());
						break;
					}
				}

				if (j < simulator.getNumAgents())
				{
					//to avoid divisions by zero in the simulation
					if (tmpRobot.getPosition() == tmpRobot.getGoal())
						tmpRobot.setGoal(tmpRobot.getGoal() + 0.0001f);
				}
				if (j < simulator.getNumGoals())
					simulator.setAgentGoal(j, hrvo::Vector2(tmpRobot.getGoal().x, tmpRobot.getGoal().y));
			}
			assignGoalIndex(assignmentMode);
		}
    }
}

//--------------------------------------------------------------
void ofApp::sendPositions()
{
    int sentBytes = udpSender.Send((char*)robotCollection.data(), sizeof(RobotState)*robotCollection.size());
}

/* The following lines of code use Hungarian Algorithm to optimize the
 * robots' final position assignment problem, which is required in this example.
 *
 * mode = 0 --> Hungarian Alg is not used and each index remains unchanged
 * mode = 1 --> Hungarian Alg assigns the indeces
 *
 */
//--------------------------------------------------------------
void ofApp::assignGoalIndex(unsigned char mode) {
    
    if (mode == 0) {
        for (int k = 0; k < robotCollection.size(); k++) {
            simulator.setAgentGoal(k, k);
            robotCollection[k].setGoal(ofVec2f(simulator.getGoalPosition(k).getX(), simulator.getGoalPosition(k).getY()));
        }
    }
    else if (mode == 1) {
        // Matrix size: robotNum * robotNum
        unsigned int size = robotCollection.size();

        vector<vector<double>> Cost(size, vector<double>(size));
        // Fill matrix with Cost values
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++) // The cost is the shortest distance to be taken by each robot
            {
				if (robotCollection[i].getTouch() == 0)
				{
					Cost[i][j] = pow(1000.0f*(robotCollection[i].getPosition() - ofVec2f(simulator.getGoalPosition(j).getX(), simulator.getGoalPosition(j).getY())).length(), 2.0f);
					
				}
				else
					Cost[i][j] = 1000000.0f;
            }
        }
        vector<int> assignment;
        AssignmentProblemSolver APS; // Creating an instance of AssignmentProblemSolver
        APS.Solve(Cost, assignment);
        for (int k = 0; k < robotCollection.size(); k++)
        {
            simulator.setAgentGoal(k, assignment[k]);
            robotCollection[k].setGoal(ofVec2f(simulator.getGoalPosition(assignment[k]).getX(), simulator.getGoalPosition(assignment[k]).getY()));
        }
    }
    else {
        std::cout << "Invalid input! Please run the program again!" << std::endl;
        throw;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    
    switch (key)
    {

            
        case '?':
        case '/':
            displayHelp = true;
            break;

		case '+':
			finalOrientation = (finalOrientation>=180.0f) ? 0.0f : finalOrientation+2.0f;
			cout << "Final Orientation: " << finalOrientation << endl;
			for (int i = 0; i < robotCollection.size(); i++)
				robotCollection[i].setAngle(finalOrientation);
			break;

		case '-':
			finalOrientation = (finalOrientation<=-180.0f) ? 0.0f : finalOrientation - 2.0f;
			cout << "Final Orientation: " << finalOrientation << endl; 
			for (int i = 0; i < robotCollection.size(); i++)
				robotCollection[i].setAngle(finalOrientation);
			break;
            
        case '1':
            robotToCommand = 1;
            break;
        case '2':
            robotToCommand = 2;
            break;
        case '3':
            robotToCommand = 3;
            break;
        case '4':
            robotToCommand = 4;
            break;
        case '5':
            robotToCommand = 5;
            break;
        case '6':
            robotToCommand = 6;
            break;
        case '7':
            robotToCommand = 7;
            break;
        case '8':
            robotToCommand = 8;
            break;
        case '9':
            robotToCommand = 9;
            break;
        case '0':
            robotToCommand = 0;
            break;
            
        case OF_KEY_UP:
            kSpeed+=0.005f;
            if(kSpeed<0.0f) kSpeed=0.0f;
            prefSpeed = kSpeed * maxSpeed;
            uncertaintyOffset = 0.05f * prefSpeed;
            break;
        case OF_KEY_DOWN:
            kSpeed-=0.005f;
            if(kSpeed<0.0f) kSpeed=0.0f;
            prefSpeed = kSpeed * maxSpeed;
            uncertaintyOffset = 0.05f * prefSpeed;
            break;
        case 'w':
            if(robotToCommand>=0 && robotToCommand < robotCollection.size())
                controlRobotSpeed(robotToCommand, 40, 40, robotCollection[robotToCommand].getColor());
            break;
        case 's':
            if (robotToCommand >= 0 && robotToCommand < robotCollection.size())
                controlRobotSpeed(robotToCommand, -40, -40, robotCollection[robotToCommand].getColor());
            break;
        case 'a':
            if (robotToCommand >= 0 && robotToCommand < robotCollection.size())
                controlRobotSpeed(robotToCommand, 0, 40, robotCollection[robotToCommand].getColor());
            break;
        case 'd':
            if (robotToCommand >= 0 && robotToCommand < robotCollection.size())
                controlRobotSpeed(robotToCommand, 40, 0, robotCollection[robotToCommand].getColor());
            break;
            
        default:
            break;
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    switch(key)
    {
        case '?':
        case '/':
            displayHelp = false;
            break;
        case ' ':
			switch (simulationMode)
			{
			case ON:
				simulationMode = OFF;
				break;
			case OFF:
				simulationMode = NO_PLANNING;
				break;
			case NO_PLANNING:
				simulationMode = ON;
				break;
			default:
				break;
			}

            break;
        case OF_KEY_RETURN:
            if(assignmentMode == 0)
                assignmentMode = 1;
            else if(assignmentMode == 1)
                assignmentMode = 0;
            
            assignGoalIndex(assignmentMode);
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            robotToCommand = -1;
            break;
		case OF_KEY_F12:
			for (int i = 0; i < swarmReceivers.size(); i++)
				swarmReceivers[i]->reset();
			break;
        default:
            
            for (int i = 0; i < robotCollection.size(); i++)
                controlRobotSpeed(robotCollection[i].getId(), 0, 0, robotCollection[i].getColor());
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    
    float mouseX = dimensionX - (float)x / ofGetWidth()*dimensionX;
    float mouseY = dimensionY - (float)y / ofGetHeight()*dimensionY;
    
    if(mouseX > dimensionX) mouseX = dimensionX;
    if(mouseX < 0.0f)       mouseX = 0.0f;
    if(mouseY > dimensionY) mouseY = dimensionY;
    if(mouseY < 0.0f)       mouseY = 0.0f;
    
    if(button == OF_MOUSE_BUTTON_1)
    {
        for (int i = 0; i <  robotCollection.size(); i++)
        {
            if(robotCollection[i].getTouch()>0 && button == OF_MOUSE_BUTTON_1)
            {
                robotCollection[i].setPosition(mouseX, mouseY);
                simulator.setAgentPosition(robotCollection[i].getId(), hrvo::Vector2(mouseX, mouseY));
            }
        }
    }
    if(goalClicked>=0 && button == OF_MOUSE_BUTTON_3)
    {
        simulator.setAgentGoal(goalClicked, hrvo::Vector2(mouseX, mouseY));
    }
    assignGoalIndex(assignmentMode);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    float mouseX = dimensionX - (float)x / ofGetWidth()*dimensionX;
    float mouseY = dimensionY - (float)y / ofGetHeight()*dimensionY;
    
	if (mouseX > dimensionX) mouseX = dimensionX;
	if (mouseX < 0.001f)     mouseX = 0.001f;
	if (mouseY > dimensionY) mouseY = dimensionY;
	if (mouseY < 0.001f)     mouseY = 0.001f;
    
    if(button == OF_MOUSE_BUTTON_1)
    {
        for(int i=0;i<robotCollection.size();i++)
        {
            float distance = (robotCollection[i].getPosition()-ofVec2f(mouseX, mouseY)).lengthSquared();
            if (distance < robotCollection[i].getRadius()*robotCollection[i].getRadius() && button == OF_MOUSE_BUTTON_1) {
                robotCollection[i].setTouch(1);
                break;
            }
            else
                robotCollection[i].setTouch(0); 
        }
    }
    else if(button == OF_MOUSE_BUTTON_3)
    {
        for(int i=0;i<simulator.getNumGoals();i++)
        {
            float distance = ofVec2f(simulator.getGoalPosition(i).getX() - mouseX, simulator.getGoalPosition(i).getY() - mouseY).lengthSquared();
            if(distance < robotRadius*robotRadius)
                goalClicked = i;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    for(int i=0;i<robotCollection.size();i++)
    {
        robotCollection[i].setTouch(0);
    }
    goalClicked = -1;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	ofSetWindowShape(w, w/1.6f);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
