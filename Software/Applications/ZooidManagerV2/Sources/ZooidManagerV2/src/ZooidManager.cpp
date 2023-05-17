//
// Created by Mathieu Le Goc on 04/02/2017.
//

#include "ZooidManager.h"

//--------------------------------------------------------------
ZooidManager::ZooidManager() {
    worldDimensions = ofVec2f(1.0f, 0.6f);
    numZooids = 20;
    updIPAddress = DEFAULT_UDP_IP;
    serverType = ServerType::NONE;
    updating = true;
}

//--------------------------------------------------------------
ZooidManager::~ZooidManager() {
    saveParameters();
    for (int i = 0; i < myReceivers.size(); i++)
        delete(myReceivers[i]);
    
    myReceivers.clear();
    updating = false;
    updateThread.join();
    myGoals.clear();
    myZooids.clear();
    closeUDPServer();
}

//--------------------------------------------------------------
void ZooidManager::init() {
    
    loadParameters();
    nbRequiredReceivers = (int)ceil((float)numZooids / (float)NUM_ZOOIDS_PER_RECEIVER);
    
    zooidArtwork.load("./graphics/zooid.svg");
    batteryArtwork.load("./graphics/battery.svg");
    lockArtwork.load("./graphics/lock.svg");
    
    initNetwork();
    initSimulation();
    initZooidReceivers();
    
    for (int i = 0; i < numZooids; i++) {
        Zooid tmpRobot = Zooid(robotRadius,
                               ofVec2f(ofRandom(0.1f, getWorldWidth() - 0.1f), ofRandom(0.1f, getWorldHeight() - 0.1f)));
        
        ZooidGoal tmpGoal = ZooidGoal(ofVec2f(ofRandom(0.1f, getWorldWidth() - 0.1f), ofRandom(0.1f, getWorldHeight() - 0.1f)),
                                      ofRandomf() * 180.0f,
                                      ofColor(ofRandomf() * 255.0f, ofRandomf() * 255.0f, ofRandomf() * 255.0f));
        
        unsigned int goalId = (unsigned int)simulator.addGoal(hrvo::Vector2(tmpGoal.getPosition().x, tmpGoal.getPosition().y));
        unsigned int robotId = (unsigned int)simulator.addAgent(hrvo::Vector2(tmpRobot.getPosition().x, tmpRobot.getPosition().y), goalId);
        
        tmpRobot.setId(robotId);
        tmpGoal.setId(goalId);
        
        tmpRobot.setGoal(&tmpGoal);
        tmpGoal.setAssociatedZooid(&tmpRobot);
        
        myZooids.push_back(tmpRobot);
        myGoals.push_back(tmpGoal);
    }
    
    assignRobots();
    
    updateThread = thread(&ZooidManager::update, this);
}

//--------------------------------------------------------------
void ZooidManager::loadParameters(){
    ofBuffer settingsBuffer = ofBufferFromFile("settings.ini");
    if(settingsBuffer.size()>0) {
        Document settings;
        if (!settings.ParseInsitu(settingsBuffer.getData()).HasParseError()){
            
            if (settings.HasMember("NbZooids")) {
                numZooids = (unsigned int)settings["NbZooids"].GetInt();
                
            }
            if (settings.HasMember("Assignment")) {
                if(settings["Assignment"].GetInt() == 0)
                    assignmentMode = AssignmentMode::NaiveAssignment;
                else if(settings["Assignment"].GetInt() == 1)
                    assignmentMode = AssignmentMode::OptimalAssignment;
            }
            if(settings.HasMember("WorldDimensions") && settings["WorldDimensions"][0].GetDouble() > 0.0 && settings["WorldDimensions"][1].GetDouble() > 0.0){
                worldDimensions.set(settings["WorldDimensions"][0].GetDouble(),settings["WorldDimensions"][1].GetDouble());
            }
            else{
                worldDimensions.set(1.0f, 0.6f);
            }
            if (settings.HasMember("ServerType")) {
                string type = settings["ServerType"].GetString();
                
                if(type == "UDP"){
                    serverType = ServerType::UDP;
                    if (settings.HasMember("UDPIP"))
                        updIPAddress = settings["UDPIP"].GetString();
                    else
                        updIPAddress = DEFAULT_UDP_IP;
                }
                else{
                    serverType = ServerType::NONE;
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ZooidManager::saveParameters() {
    StringBuffer settings;
    settings.Reserve(1000);
    Writer<StringBuffer> writer(settings);
    writer.SetMaxDecimalPlaces(3);

    writer.StartObject();
    {
        writer.Key("NbZooids");
        writer.Int(static_cast<int>(myZooids.size()));
        writer.Key("Assignment");
        writer.Int(assignmentMode);
        
        switch(serverType){
            case ServerType::UDP :
                writer.Key("ServerType");
                writer.String("UDP");
                writer.Key("UDPIP");
                writer.String(updIPAddress.c_str());
                break;
            case ServerType::NONE:
            default:
                writer.Key("ServerType");
                writer.String("NONE");
                break;
        }
    }
    
    writer.Key("WorldDimensions");
    writer.StartArray();
    writer.Double(worldDimensions.x);
    writer.Double(worldDimensions.y);
    writer.EndArray();

    
    writer.EndObject();
    
    ofBuffer buffer(settings.GetString(), settings.GetLength());
    ofBufferToFile("settings.ini", buffer, false);
}

//--------------------------------------------------------------
bool ZooidManager::initNetwork() {
    bool initOk = true;
    
    switch(serverType){
        case ServerType::UDP:
            initOk |= initUDPServer();
            break;
        case ServerType::NONE:
        default:
            break;
    }
    
    return initOk;
}

//--------------------------------------------------------------
bool ZooidManager::initUDPServer(){
    bool initOk = true;
    //create the socket and set to send to 127.0.0.1:11999
    initOk |= udpSender.Create();
    initOk |= udpSender.Connect(updIPAddress.c_str(), DEFAULT_UDP_SENDING_PORT);
    initOk |= udpSender.SetNonBlocking(true);
    
    initOk |= udpReceiver.Create();
    initOk |= udpReceiver.Bind(DEFAULT_UDP_RECIEVING_PORT);
    initOk |= udpReceiver.SetNonBlocking(true);
    
    return initOk;
}

//--------------------------------------------------------------
void ZooidManager::closeUDPServer(){
    if(udpSender.HasSocket())
        udpSender.Close();
    if(udpReceiver.HasSocket())
        udpReceiver.Close();
}

//--------------------------------------------------------------
ServerType ZooidManager::getServerType(){
    return serverType;
}

//--------------------------------------------------------------
void ZooidManager::setServerType(ServerType type){
    
    switch(type){
        case ServerType::UDP:
            initUDPServer();
            break;
        case ServerType::NONE:
            closeUDPServer();
            break;
        default:
            break;
    }
    serverType = type;
    
}

//--------------------------------------------------------------
string ZooidManager::getUDPIPaddress(){
    return updIPAddress;
}

//--------------------------------------------------------------
void ZooidManager::setUDPIPaddress(string ipAddress){
    updIPAddress = ipAddress;
}

//--------------------------------------------------------------
bool ZooidManager::initSimulation() {
    simulationMode = On;
    
    kSpeed = 0.35f;
    prefSpeed = 0.6f * maxSpeed;
    uncertaintyOffset = 0.05f * prefSpeed;
    
    simulator.setTimeStep(UPDATE_PERIOD / 1000.0f);
    simulator.setAgentDefaults(neighborDist, maxNeighbors, robotRadius, goalRadius, prefSpeed, maxSpeed,
                               TIME_TO_ORIENTATION, wheelTrack, uncertaintyOffset, maxAccel, hrvo::Vector2(0.0f, 0.0f),
                               0.0f);
    
    return true;
}

//--------------------------------------------------------------
bool ZooidManager::initZooidReceivers() {
    bool initOk = true;
    
    vector<string> serialPorts = getAvailableZooidReceivers();
    int receiversToConnect = min(nbRequiredReceivers, (int)serialPorts.size());
    
    for (int i = 0; i < serialPorts.size(); i++) {
        if (myReceivers.size() < receiversToConnect) {
            ZooidReceiver* z = new ZooidReceiver(static_cast<int>(myReceivers.size()));
            if (z->init(serialPorts[i]) == true) {
                myReceivers.push_back(z);
                
                // NEED THIS SLEEP TO RECEIVE THE HANDSHAKE RESPONSE, DONT KNOW WHY -- FIGURE IT OUT
                ofSleepMillis(1);
                
            }
        }
        else
            break;
    }
    return initOk;
}

//--------------------------------------------------------------
void ZooidManager::update() {
    
    uint64_t previousTimestep = ofGetElapsedTimeMicros();
    
    while (updating) {
        
        receiveClientInstructions();
        processReceiversData();
        
        float elapsedTime = (float)(ofGetElapsedTimeMicros() - previousTimestep) / 1000.0f;
        
        if (elapsedTime >= UPDATE_PERIOD) {
            runSimulation();
            sendRobotsOrders();
            sendClientInformation();
            
            previousTimestep = ofGetElapsedTimeMicros();
            
            unique_lock<mutex> lock(valuesMutex);
            {
                currentUpdatePeriod = elapsedTime;
            }
            lock.unlock();
        }
        else
            ofSleepMillis(1);
    }
}

//--------------------------------------------------------------
bool ZooidManager::runSimulation() {
    SimulationMode mode;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        mode = simulationMode;
    }
    lock.unlock();
    
    //running one step of the simulation to compute new directions and motors values
    //has to run at the given frequency
    
    if (mode == On) {
        //first update the simulation with the latest robots' positions and orientations
//        for (int i = 0; i < myZooids.size(); i++) {
//            if(myZooids[i].isConnected()){
//                simulator.setAgentPosition(myZooids[i].getId(), hrvo::Vector2(myZooids[i].getPosition().x, myZooids[i].getPosition().y));
//                simulator.setAgentOrientation(myZooids[i].getId(), myZooids[i].getOrientation() * PI / 180.0f);
//            }
//        }
        //then update the goals' positions
        
        for (int i = 0; i < myGoals.size(); i++) {
            unique_lock<mutex> lock(valuesMutex);
            {
                simulator.setGoalPosition(myGoals[i].getId(), hrvo::Vector2(myGoals[i].getPosition().x, myGoals[i].getPosition().y));
            }
            lock.unlock();
        }
        
        //then do the HRVO magic
        unique_lock<mutex> lock(valuesMutex);
        {
            simulator.doStep();
        }
        lock.unlock();
        
        for (int i = 0; i < myZooids.size(); i++) {
            unique_lock<mutex> lock(valuesMutex);
            {
                //trick to slow them down around th goal, figure out something betteR
                hrvo::Vector2 distance = simulator.getAgentPosition(i) - simulator.getGoalPosition(simulator.getAgentGoal(i));
                
                float k = pow(abs(distance), 2.0f) * 2000.0f + 0.000001f; // #TODO : find the reason for the k=0 problem
                if (k >= 1.0f) k = 1.0f;
                
                
                //                if (simulator.getAgentReachedGoal(i))
                //                    simulator.setAgentPrefSpeed(i, 0.0f);
                //                else
                {
                    simulator.setAgentPrefSpeed(i, prefSpeed * k * (float)myZooids[i].getSpeed()/100.0f);
                    simulator.setAgentMaxSpeed(i, 1.1f * prefSpeed * k);
                }
                
                if(myZooids[i].getState() == 0){
                    if (myZooids[i].isActivated() && !myZooids[i].isConnected()) {
                        if(simulator.getAgentPosition(i).getX() >= 0.0f && simulator.getAgentPosition(i).getY() >= 0.0f){
                            myZooids[i].setPosition(simulator.getAgentPosition(i).getX(), simulator.getAgentPosition(i).getY());
                            float angle = simulator.getAgentOrientation(i) * 180.0f / PI;
                            angle = (angle>=0.0f) ? fmod(angle, 360.0f) : fmod(angle, -360.0f);
                            myZooids[i].setOrientation(angle);
                            myZooids[i].setGoalReached(simulator.getAgentReachedGoal(i));
                        }
                        myZooids[i].setGoalReached(simulator.getAgentReachedGoal(i));
                    }
                }
//                else //to avoid having the zooid teleport when the mouse is released
//                    simulator.setAgentPosition(i, hrvo::Vector2(myZooids[i].getPosition().x, myZooids[i].getPosition().y));
                
            }
            lock.unlock();
        }
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void ZooidManager::assignRobots() {
    AssignmentMode mode;
    unique_lock<mutex> lock(valuesMutex);
    {
        mode = assignmentMode;
    }
    lock.unlock();
    
    if (mode == NaiveAssignment) {
        for (unsigned long k = 0; k < myZooids.size(); k++) {
            unique_lock<mutex> lock(valuesMutex);
            {
                simulator.setAgentGoal(k, k);
                myZooids[k].setColor(myGoals[k].getColor());
                myGoals[k].setAssociatedZooid(&myZooids.at(k));
            }
            lock.unlock();
        }
    }
    else if (mode == OptimalAssignment) {
        // Matrix size: robotNum * robotNum
        unsigned long size = min(myGoals.size(), myZooids.size());
        
        vector<vector<double>> Cost(size, vector<double>(size));
        // Fill matrix with Cost values
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) { // The cost is the shortest distance to be taken by each zooid
                Cost[i][j] = (myGoals[j].getPosition() - myZooids[i].getPosition()).lengthSquared() * 1000.0;
                
                if (myGoals[j].isReassignable() == false || myGoals[j].getAssociatedZooid()->getState() > 0) {
                    if (i != myGoals[j].getAssociatedZooid()->getId())
                        Cost[i][j] *= 100000000.0;
                    else
                        Cost[i][j] /= 1000.0;
                }
            }
        }
        
        vector<int> assignment;
        AssignmentProblemSolver APS; // Creating an instance of AssignmentProblemSolver
        APS.Solve(Cost, assignment);
        
        for (int i = 0; i < size; i++) {
            unique_lock<mutex> lock(valuesMutex);
            {
                simulator.setAgentGoal(i, assignment[i]);
                myZooids[i].setColor(myGoals[assignment[i]].getColor());
                myGoals[assignment[i]].setAssociatedZooid(&myZooids[i]);
            }
            lock.unlock();
        }
    }
}

//--------------------------------------------------------------
bool ZooidManager::receiveClientInstructions() {
    static char udpMessage[100000] = { 0 };
    
    if(serverType == ServerType::UDP && udpReceiver.PeekReceive() > 0) {
        int nbBytesReceived = udpReceiver.Receive(udpMessage, 100000);
        if(nbBytesReceived > 0 && processClientInstructions(udpMessage)) {
            assignRobots();
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::processClientInstructions(char* message) {
    Document zooidData;
    
    if (message != NULL && zooidData.ParseInsitu(message).HasParseError())
        return false;
    
    int nbZooids = zooidData["nb"].GetInt();
    assignmentMode = (AssignmentMode)zooidData["ass"].GetInt();
    
    const Value& receivedZooids = zooidData["zoo"]; // Using a reference for consecutive access is handy and faster.
    assert(receivedZooids.IsArray());
    if (receivedZooids.Size() == nbZooids) {
        for (SizeType i = 0; i < receivedZooids.Size(); i++) {
            unsigned int tmpId = (unsigned int)receivedZooids[i]["id"].GetInt();
            
            auto it = find_if(myGoals.begin(), myGoals.end(), [&tmpId](ZooidGoal &g) { return g.getId() == tmpId; });
            if (it != myGoals.end()) {
                
                unique_lock<mutex> lock(valuesMutex);
                {
                    if (receivedZooids[i].HasMember("des")) {
                        ofVec2f destination((float)receivedZooids[i]["des"][0].GetDouble(), (float)receivedZooids[i]["des"][1].GetDouble());
                        
                        destination.x = (destination.x < (robotDiameter + robotFieldMargin)) ? (robotDiameter + robotFieldMargin) : destination.x;
                        destination.x = (destination.x > (getWorldWidth() - robotDiameter - robotFieldMargin)) ? (getWorldWidth() - robotDiameter - robotFieldMargin) : destination.x;
                        destination.y = (destination.y < (robotDiameter + robotFieldMargin)) ? (robotDiameter + robotFieldMargin) : destination.y;
                        destination.y = (destination.y > (getWorldHeight() - robotDiameter - robotFieldMargin)) ? (getWorldHeight() - robotDiameter - robotFieldMargin) : destination.y;
                        
                        if (ofVec2f(destination - it->getAssociatedZooid()->getPosition()).length()<0.001f)
                            it->setPosition(destination + 0.001f);
                        else
                            it->setPosition(destination);
                    }
                    
                    if (receivedZooids[i].HasMember("col")) {
                        it->setColor(ofColor(receivedZooids[i]["col"][0].GetInt(), receivedZooids[i]["col"][1].GetInt(), receivedZooids[i]["col"][2].GetInt()));
                    }
                    
                    if (receivedZooids[i].HasMember("ang")) {
                        float tmpOrientation = (float)receivedZooids[i]["ang"].GetDouble();
                        tmpOrientation = (tmpOrientation > 360.0f) ? 360.0f : tmpOrientation;
                        tmpOrientation = (tmpOrientation < -360.0f) ? -360.0f : tmpOrientation;
                        it->setOrientation(tmpOrientation);
                    }
                    
                    if (receivedZooids[i].HasMember("rea"))
                        it->setReassignable(receivedZooids[i]["rea"].GetBool());
                    
                    if (receivedZooids[i].HasMember("vel")) {
                        unsigned int tmpSpeed = (unsigned int)receivedZooids[i]["vel"].GetInt();
                        tmpSpeed = (tmpSpeed >= 100) ? 100 : tmpSpeed;
                        it->getAssociatedZooid()->setSpeed(tmpSpeed);
                    }
                    if(receivedZooids[i].HasMember("act")){
                        if (receivedZooids[i]["act"].GetBool())
                            it->getAssociatedZooid()->activate();
                        else
                            it->getAssociatedZooid()->deactivate();
                    }
                }
                lock.unlock();
            }
        }
        return true;
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidManager::sendClientInformation() {
    
    StringBuffer s;
    s.Reserve(10000);
    Writer<StringBuffer> writer(s);
    writer.SetMaxDecimalPlaces(3);
    writer.StartObject();
    {
        unique_lock<mutex> lock(valuesMutex);
        writer.Key("ass");          //assignation strategy
        writer.Int(assignmentMode);
        lock.unlock();
        
        writer.Key("nb");           //number of zooids
        writer.Int(static_cast<int>(myGoals.size()));
        
        writer.Key("dim");          //dimensions of the projection space for position mapping
        writer.StartArray();
        writer.Double(getWorldWidth());
        writer.Double(getWorldHeight());
        writer.EndArray();
        
        writer.Key("zoo");          //array of zooids
        writer.StartArray();
        {
            for (unsigned int i = 0; i < myGoals.size(); i++) {
                writer.StartObject();
                writer.Key("id");   //zooid id
                writer.Int(myGoals[i].getId());
                writer.Key("siz");  //zooid size
                writer.Double(robotRadius);
                writer.Key("ang");  //zooid angle
                writer.Double(myGoals[i].getAssociatedZooid()->getOrientation());
                if(myGoals[i].getAssociatedZooid()->getPosition().x>=0.0f && myGoals[i].getAssociatedZooid()->getPosition().y>=0.0f){
                    writer.Key("pos");  // zooid position
                    writer.StartArray();
                    writer.Double(myGoals[i].getAssociatedZooid()->getPosition().x);
                    writer.Double(myGoals[i].getAssociatedZooid()->getPosition().y);
                    writer.EndArray();
                }
                
                writer.Key("des");  //zooid destination
                writer.StartArray();
                writer.Double(myGoals[i].getPosition().x);
                writer.Double(myGoals[i].getPosition().y);
                writer.EndArray();
                writer.Key("sta");  //zooid interaction state
                writer.Int(myGoals[i].getAssociatedZooid()->getState());
                writer.Key("her");  //zooid at goal
                writer.Bool(myGoals[i].getAssociatedZooid()->isGoalReached());
                writer.Key("col");  //zooid color
                writer.StartArray();
                writer.Int(myGoals[i].getColor().r);
                writer.Int(myGoals[i].getColor().g);
                writer.Int(myGoals[i].getColor().b);
                writer.EndArray();
                writer.Key("act");  //zooid activated
                writer.Bool(myGoals[i].getAssociatedZooid()->isActivated());
                writer.Key("rea");  //zooid reassisgnable
                writer.Bool(myGoals[i].isReassignable());
                writer.Key("vel");  //zooid speed
                writer.Int(myGoals[i].getAssociatedZooid()->getSpeed());
                writer.EndObject();
            }
        }
        writer.EndArray();
    }
    writer.EndObject();
    
    if(serverType == ServerType::UDP){
        int bytesSent = udpSender.Send(s.GetString(), static_cast<int>(s.GetSize()));
        return bytesSent != SOCKET_ERROR ? true : false;
    }

	else return false;
}

//--------------------------------------------------------------
void ZooidManager::processReceiversData() {
    
    unique_lock<mutex> lock(valuesMutex);
    {
        for(auto &z:myZooids)
            z.tickWatchdog();
    }
    lock.unlock();
    
    //updating the positions with the data received from robots
    for (int i = 0; i < myReceivers.size(); i++) {
        while (myReceivers[i]->availableMessages() > 0) {
            Message msg = myReceivers[i]->getLastMessage();
            unsigned int tmpId = msg.getSenderId() + myReceivers[i]->getId() * NUM_ZOOIDS_PER_RECEIVER;
            
            if (tmpId < myZooids.size() && msg.getType() == TYPE_STATUS) {
                StatusMessage status;
                memcpy(&status, msg.getPayload(), sizeof(StatusMessage));
                
                float robotA = (float)(status.orientation) / 100.0f;
                float robotX = ofMap((float)status.positionX, coordinatesMinX, coordinatesMaxX, 0.0f, getWorldWidth(), true);
                float robotY = ofMap((float)status.positionY, coordinatesMinY, coordinatesMaxY, 0.0f, getWorldHeight(), true);
                
                unique_lock<mutex> lock(valuesMutex);
                {
                    myZooids[tmpId].lastUpdate = ofGetElapsedTimeMillis();
                    myZooids[tmpId].setState(status.state);
                    myZooids[tmpId].setPosition(robotX, robotY);
                    myZooids[tmpId].setOrientation(robotA);
                    myZooids[tmpId].setBatteryLevel(status.batteryLevel);
                    myZooids[tmpId].resetWatchdog();
                }
                lock.unlock();
            }
        }
    }
}

//--------------------------------------------------------------
void ZooidManager::controlRobotSpeed(int id, int8_t motor1, int8_t motor2, ofColor color) {
    uint8_t buffer[5] = { 0 };
    
    if (id >= 0 && id < myZooids.size()) {
        buffer[0] = (motor1 <= 100) ? motor1 : 100;
        buffer[1] = (motor2 <= 100) ? motor2 : 100;
        buffer[2] = color.r;
        buffer[3] = color.g;
        buffer[4] = color.b;
        
        ZooidReceiver* r = retrieveReceiver(id);
        if (r && r->isInitialized())
            r->sendUSB(TYPE_MOTORS_VELOCITY, myZooids[id].getId() - r->getId()*NUM_ZOOIDS_PER_RECEIVER, sizeof(buffer), (uint8_t *)&buffer[0]);
    }
}

//--------------------------------------------------------------
void ZooidManager::controlRobotPosition(uint8_t id, float x, float y, ofColor color, float orientation, float preferredSpeed, bool isFinalGoal) {
    PositionControlMessage msg;
    
    if (id >= 0 && id < myZooids.size()) {
        float tmpX = x, tmpY = y;
        if (tmpX > getWorldWidth()) tmpX = getWorldWidth();
        if (tmpX < 0.0f) tmpX = 0.0f;
        if (tmpY > getWorldHeight()) tmpY = getWorldHeight();
        if (tmpY < 0.0f) tmpX = 0.0f;
        
        msg.positionX = (uint16_t)ofMap(tmpX, 0.0f, getWorldWidth(), coordinatesMinX, coordinatesMaxX);
        msg.positionY = (uint16_t)ofMap(tmpY, 0.0f, getWorldHeight(), coordinatesMinY, coordinatesMaxY);
        msg.colorRed = color.r;
        msg.colorGreen = color.g;
        msg.colorBlue = color.b;
        msg.orientation = (uint16_t)(orientation * 100.0f);
        msg.preferredSpeed = (uint8_t)preferredSpeed;
        msg.isFinalGoal = isFinalGoal;
        msg.empty = 0xff;
        
        ZooidReceiver* r = retrieveReceiver(id);
        if (r && r->isInitialized()){
            r->sendUSB(TYPE_ROBOT_POSITION, myZooids[id].getId() - r->getId()*NUM_ZOOIDS_PER_RECEIVER, sizeof(msg), (uint8_t *)&msg);
        }
    }
}

//--------------------------------------------------------------
void ZooidManager::sendRobotsOrders() {
    SimulationMode mode;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        mode = simulationMode;
    }
    lock.unlock();
    
    
    //running one step of the simulation to compute new directions and motors values
    //has to run at the given frequency
    
    for (int i = 0; i < myZooids.size(); i++) {
        if (!myZooids[i].isActivated())
            controlRobotSpeed(myZooids[i].getId(), 0, 0, myZooids[i].getColor());
        else if (mode == On) {
            controlRobotPosition(myZooids[i].getId(),
                                 simulator.getAgentPosition(myZooids[i].getId()).getX(),
                                 simulator.getAgentPosition(myZooids[i].getId()).getY(),
                                 myZooids[i].getColor(),
                                 retrieveAssociatedGoal(i)->getOrientation(),
                                 myZooids[i].getSpeed(),
                                 myZooids[i].isGoalReached());
        }
        else if (mode == NoPlanning) {
            controlRobotPosition(myZooids[i].getId(),
                                 retrieveAssociatedGoal(i)->getPosition().x,
                                 retrieveAssociatedGoal(i)->getPosition().y,
                                 //                                             simulator.getGoalPosition(simulator.getAgentGoal(myZooids[i].getId())).getX(),
                                 //                                             simulator.getGoalPosition(simulator.getAgentGoal(myZooids[i].getId())).getY(),
                                 myZooids[i].getColor(),
                                 retrieveAssociatedGoal(i)->getOrientation(),
                                 myZooids[i].getSpeed(),
                                 myZooids[i].isGoalReached());
        }
    }
    
    for (int j = 0; j < myReceivers.size(); j++)
        myReceivers[j]->setReadyToSend();
}

//--------------------------------------------------------------
SimulationMode ZooidManager::getSimulationMode() {
    SimulationMode mode;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        mode = simulationMode;
    }
    
    return mode;
}

//--------------------------------------------------------------
void ZooidManager::setSimulationMode(SimulationMode mode) {
    
    unique_lock<mutex> lock(valuesMutex);
    {
        simulationMode = mode;
    }
    lock.unlock();
}

//--------------------------------------------------------------
AssignmentMode ZooidManager::getAssignmentMode() {
    AssignmentMode mode;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        mode = assignmentMode;
    }
    lock.unlock();
    
    return mode;
}

//--------------------------------------------------------------
void ZooidManager::setAssignmentMode(AssignmentMode mode) {
    unique_lock<mutex> lock(valuesMutex);
    {
        assignmentMode = mode;
    }
    lock.unlock();
    assignRobots();
}

//--------------------------------------------------------------
float ZooidManager::getWorldWidth(){
    return worldDimensions.x;
}

//--------------------------------------------------------------
float ZooidManager::getWorldHeight(){
    return worldDimensions.y;
}

//--------------------------------------------------------------
void ZooidManager::setWorldWidth(float w){
    if(w > 0.0f)
        worldDimensions.x = w;
}

//--------------------------------------------------------------
void ZooidManager::setWorldHeight(float h){
    if(h > 0.0f)
        worldDimensions.y = h;
}

//--------------------------------------------------------------
void ZooidManager::setWorldDimensions(float w, float h){
    if(w > 0.0f && h >0.0f)
        worldDimensions.set(w, h);
}

//--------------------------------------------------------------
Zooid* ZooidManager::getZooidFromId(unsigned int zooidId) {
    Zooid* zooid = nullptr;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        for(int i=0;i<myZooids.size();i++){
            if(myZooids[i].getId() == zooidId)
                zooid = &myZooids[i];
        }
    }
    lock.unlock();
    
    return zooid;
}

//--------------------------------------------------------------
ZooidGoal* ZooidManager::getGoalFromId(unsigned int goalId) {
    ZooidGoal* goal = nullptr;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        for(int i=0;i<myGoals.size();i++){
            if(myGoals[i].getId() == goalId)
                goal = &myGoals[i];
        }
    }
    lock.unlock();
    
    return goal;
}

//--------------------------------------------------------------
Zooid ZooidManager::getZooid(unsigned int index) {
    Zooid z;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myZooids.size())
            z = myZooids[index];
        else
            z = myZooids[myZooids.size() - 1];
    }
    lock.unlock();
    
    return (index < myZooids.size()) ? myZooids[index] : myZooids[myZooids.size() - 1];
}

//--------------------------------------------------------------
ZooidGoal ZooidManager::getGoal(unsigned int index) {
    ZooidGoal goal;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myGoals.size())
            goal = myGoals[index];
        else
            goal = myGoals[myGoals.size() - 1];
    }
    lock.unlock();
    
    return goal;
}

//--------------------------------------------------------------
int ZooidManager::getNbGoals() {
    int nb = 0;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        nb = static_cast<int>(myGoals.size());
    }
    lock.unlock();
    
    return nb;
}

//--------------------------------------------------------------
int ZooidManager::getNbZooids() {
    int nb = 0;
    
    unique_lock<mutex> lock(valuesMutex);
    {
        nb = static_cast<int>(myZooids.size());
    }
    lock.unlock();
    
    return nb;
}

//--------------------------------------------------------------
void ZooidManager::setZooidInteraction(unsigned int index, bool touched, bool blinded, bool tapped, bool shaken) {
    
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myZooids.size()) {
            int tmpState = 0;
            if (touched) tmpState |= 1;
            if (blinded) tmpState |= 2;
            if (tapped)  tmpState |= 4;
            if (shaken)  tmpState |= 8;
            
            myZooids[index].setState(tmpState);
        }
    }
    lock.unlock();
}

//--------------------------------------------------------------
void ZooidManager::moveZooid(unsigned int index, ofVec2f position) {
    
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myZooids.size()) {
            myZooids[index].setPosition(position);
            simulator.setAgentPosition(myZooids[index].getId(), hrvo::Vector2(position.x, position.y));
        }
    }
    lock.unlock();
}

//--------------------------------------------------------------
void ZooidManager::moveZooid(unsigned int index, float x, float y) {
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myZooids.size()) {
            myZooids[index].setPosition(x, y);
            simulator.setAgentPosition(myZooids[index].getId(), hrvo::Vector2(x, y));
        }
    }
    lock.unlock();
}

//--------------------------------------------------------------
void ZooidManager::rotateZooid(unsigned int index, float angle) {
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myZooids.size()) {
            myZooids[index].setOrientation(myZooids[index].getOrientation() + angle);
        }
    }
    lock.unlock();
    
}

//--------------------------------------------------------------
void ZooidManager::moveGoal(unsigned int index, ofVec2f position) {
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myGoals.size())
            myGoals[index].setPosition(position);
    }
    lock.unlock();
}

//--------------------------------------------------------------
void ZooidManager::moveGoal(unsigned int index, float x, float y) {
    unique_lock<mutex> lock(valuesMutex);
    {
        if (index < myGoals.size())
            myGoals[index].setPosition(x, y);
    }
    lock.unlock();
}

//--------------------------------------------------------------
float ZooidManager::getUpdateFrequency() {
    
    float f = 0.0f;
    unique_lock<mutex> lock(valuesMutex);
    {
        f = 1000.0f / currentUpdatePeriod;
    }
    lock.unlock();
    
    return f;
}

//--------------------------------------------------------------
bool ZooidManager::isReceiverConnected() {
    return receiverConnected;
}

//--------------------------------------------------------------
int ZooidManager::getNbConnectedReceivers() {
    return static_cast<int>(myReceivers.size());
}

//--------------------------------------------------------------
int ZooidManager::getNbRequiredReceivers() {
    return nbRequiredReceivers;
}

//--------------------------------------------------------------
ZooidGoal* ZooidManager::retrieveAssociatedGoal(unsigned int zooidId) {
    if (zooidId < myZooids.size()) {
        auto it = find_if(myGoals.begin(), myGoals.end(), [&zooidId](ZooidGoal &g) { return g.getAssociatedZooid()->getId() == zooidId; });
        if (it != myGoals.end()) {
            return &myGoals[it->getId()];
        }
    }
    return &myGoals[0];
}

//--------------------------------------------------------------
ZooidReceiver* ZooidManager::retrieveReceiver(unsigned int zooidId) {
    
    int receiverId = zooidId / NUM_ZOOIDS_PER_RECEIVER;
    if (receiverId < myReceivers.size()) {
        for (int i = 0; i < myReceivers.size(); i++) {
            if (myReceivers[i]->getId() == receiverId)
                return myReceivers[i];
        }
    }
    return nullptr;
}

//--------------------------------------------------------------
void ZooidManager::drawZooids() {
    
    ofFill();
    ofSetColor(ofColor::darkGrey, 70);
    for (int i = 0; i < myZooids.size(); i++)
        ofDrawCircle(myZooids[i].getPosition().x,
                     myZooids[i].getPosition().y,
                     robotRadius);
    
    for (int i = 0; i < myGoals.size(); i++) {
        myGoals[i].drawGoal(robotRadius);
        if (!myGoals[i].isReassignable())
            myGoals[i].drawLock(&lockArtwork, robotRadius, 1.0f);
    }
    
    for (int i = 0; i < myZooids.size(); i++) {
        myZooids[i].drawRobot(&zooidArtwork, 1.0f);
        myZooids[i].drawBatteryLevel(&batteryArtwork, 1.0f);
    }
}

//--------------------------------------------------------------
void ZooidManager::addZooid() {
    Zooid tmpZooid = Zooid(robotRadius,
                           ofVec2f(ofRandom(0.1f, getWorldWidth() - 0.1f), ofRandom(0.1f, getWorldHeight() - 0.1f)));
    
    ZooidGoal tmpGoal = ZooidGoal(ofVec2f(ofRandom(0.1f, getWorldWidth() - 0.1f), ofRandom(0.1f, getWorldHeight() - 0.1f)),
                                  ofRandomf() * 180.0f,
                                  ofColor(ofRandomf() * 255.0f, ofRandomf() * 255.0f, ofRandomf() * 255.0f));
    
    unsigned int goalId = (unsigned int)simulator.addGoal(hrvo::Vector2(tmpGoal.getPosition().x, tmpGoal.getPosition().y));
    unsigned int zooidId = (unsigned int)simulator.addAgent(hrvo::Vector2(tmpZooid.getPosition().x, tmpZooid.getPosition().y), goalId);
    
    tmpZooid.setId(zooidId);
    tmpGoal.setId(goalId);
    
    unique_lock<mutex> lock(valuesMutex);
    {
        myZooids.push_back(tmpZooid);
        myGoals.push_back(tmpGoal);
        myZooids.back().setGoal(&myGoals.back());
        myGoals.back().setAssociatedZooid(&myZooids.back());
    }
    lock.unlock();
    
    assignRobots();
}

//--------------------------------------------------------------
void ZooidManager::removeZooid() {
    
    if(myZooids.size()>1 && myGoals.size()>1){
        // remove the agent associated to the last goal
        
        unique_lock<mutex> lock(valuesMutex);
        {
            unsigned int tmpId = myGoals.back().getAssociatedZooid()->getId();
            simulator.eraseAgent(tmpId);
                       
            auto it = find_if(myZooids.begin(), myZooids.end(), [&tmpId](Zooid &z) { return z.getId() == tmpId; });
            if (it != myZooids.end())
                myZooids.erase(it);
            
            // remove goal
            simulator.eraseGoal(myGoals.back().getId());
            myGoals.pop_back();
        }
        lock.unlock();
        
        assignRobots();
    }
}

//--------------------------------------------------------------
bool ZooidManager::isZooidConnected(unsigned int index){
    if (index < myZooids.size())
        return myZooids[index].isConnected();
    else
        return false;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidTouched(unsigned int index){
    if (index < myZooids.size())
        return myZooids[index].isTouched();
    else
        return false;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidBlinded(unsigned int index){
    if (index < myZooids.size())
        return myZooids[index].isBlinded();
    else
        return false;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidTapped(unsigned int index){
    if (index < myZooids.size())
        return myZooids[index].isTapped();
    else
        return false;
}

//--------------------------------------------------------------
bool ZooidManager::isZooidShaken(unsigned int index){
    if (index < myZooids.size())
        return myZooids[index].isShaken();
    else
        return false;
}

//--------------------------------------------------------------
vector<string> ZooidManager::getAvailableZooidReceivers() {
    vector<string> descriptors;
    ofSerial serialPort;
    
    vector<ofSerialDeviceInfo> devices = serialPort.getDeviceList();
    
    
    for (int i = 0; i<devices.size(); i++) {
        string currentDevice = devices[i].getDeviceName();
#ifdef TARGET_WIN32
        if (currentDevice.find("COM") != -1)
            descriptors.push_back(currentDevice.substr(currentDevice.find('(') + 1,
                                                       currentDevice.length() - (currentDevice.find('(') + 1)));
#else
        if (currentDevice.find("tty.usbmodem") != -1)
            descriptors.push_back(currentDevice);
#endif
        
    }
    return descriptors;
}
