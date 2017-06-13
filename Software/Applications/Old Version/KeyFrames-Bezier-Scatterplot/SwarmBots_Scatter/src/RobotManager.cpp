#include "RobotManager.h"

//--------------------------------------------------------------
RobotManager::RobotManager(){

}
//--------------------------------------------------------------
RobotManager::~RobotManager(){

}

//--------------------------------------------------------------
void RobotManager::initialize(){
	udpSender.Create();
	udpSender.Connect("127.0.0.1", 11998);
	udpSender.SetNonBlocking(true);

	udpReceiver.Create();
	udpReceiver.Bind(11999);
	udpReceiver.SetNonBlocking(true);

	robots = vector<RobotState>();
}

//--------------------------------------------------------------
bool RobotManager::updateRobot(unsigned int id, ofVec2f position, float orientation, ofColor color){
	if (id < robots.size())
	{
		robots[id].setGoal(position);
		robots[id].setOrientation(orientation);
		robots[id].setColor(color);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool RobotManager::updateRobot(unsigned int id, ofVec2f position, float orientation) {
	if (id < robots.size())
	{
		robots[id].setGoal(position);
		robots[id].setOrientation(orientation);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool RobotManager::updateRobot(unsigned int id, ofVec2f position, ofColor color) {
	if (id < robots.size())
	{
		robots[id].setGoal(position);
		robots[id].setColor(color);
		return true;
	}
	return false;
}

//--------------------------------------------------------------
bool RobotManager::sendRobotUpdates() {
	int bytesSent = udpSender.Send((char*)robots.data(), sizeof(RobotState)*robots.size());
	if (bytesSent != SOCKET_ERROR)
		return true;
	else
		return false;
}

//--------------------------------------------------------------
bool RobotManager::receiveRobotInformation(){
	char udpMessage[10000] = { 0 };

	if (udpReceiver.PeekReceive() > 0)
	{
		while (udpReceiver.PeekReceive() > 0)
		{
			int nbBytesReceived = udpReceiver.Receive(udpMessage, 10000);
			for (int i = 0; i < nbBytesReceived / sizeof(RobotState); i++)
			{
				bool foundRobot = false;
				RobotState tmpRobot;
				memcpy(&tmpRobot, &udpMessage[i*sizeof(RobotState)], sizeof(RobotState));
				for (int i = 0; i<robots.size(); i++)
				{
					unsigned int tmpId = tmpRobot.getId();
					auto it = find_if(robots.begin(), robots.end(), [&tmpId](RobotState& r) {return r.getId() == tmpId; });
					if (it != robots.end())
					{
						it->setPosition(tmpRobot.getPosition());
						it->setOrientation(tmpRobot.getOrientation());
						it->setTouch(tmpRobot.getTouch());
						foundRobot = true;
						break;
					}
				}
				if (!foundRobot) {
					robots.push_back(tmpRobot);
				}
			}
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------
vector<RobotState>* RobotManager::getRobots() {
	return &robots;
}

//--------------------------------------------------------------
RobotState::RobotState() {
	id = 0;
	position = ofVec2f(0.0f, 0.0f);
	radius = 0.01f;
	orientation = 0.0f;
	goal = ofVec2f(0.0f, 0.0f);
	touch = 0;
	goalReached = false;
	color = ofColor::green;
	batteryLevel = 100;
}

//--------------------------------------------------------------
RobotState::~RobotState(){

}

//--------------------------------------------------------------
RobotState::RobotState(unsigned int _id, ofVec2f _position, ofVec2f _goal, ofColor _color) {
	position = _position;
	radius = 0.01f;
	orientation = 0.0f;
	goal = _goal;
	touch = NO_TOUCH;
	goalReached = false;
	color = _color;
	batteryLevel = 0;
	id = _id;
}

//--------------------------------------------------------------
bool RobotState::operator==(const RobotState& r){
	return  this->id == r.id &&
		this->position == r.position &&
		this->radius == r.radius &&
		this->orientation == r.orientation &&
		this->goal == r.goal  &&
		this->goalReached == r.goalReached  &&
		this->touch == r.touch &&
		this->color == r.color &&
		this->batteryLevel == r.batteryLevel;
}

//--------------------------------------------------------------
bool RobotState::operator!=(const RobotState& r) {
	return !(*this == r);
}

//--------------------------------------------------------------
ofVec2f RobotState::getPosition() {
	return position;
}

//--------------------------------------------------------------
float RobotState::getOrientation() {
	return orientation;
}

//--------------------------------------------------------------
uint8_t RobotState::getTouch() {
	return touch;
}

//--------------------------------------------------------------
bool RobotState::isGoalReached() {
	return goalReached;
}

//--------------------------------------------------------------
unsigned int RobotState::getId() {
	return id;
}

//--------------------------------------------------------------
float RobotState::getRadius() {
	return radius;
}

//--------------------------------------------------------------
void RobotState::setPosition(ofVec2f _pos) {
	position = _pos;
}

//--------------------------------------------------------------
void RobotState::setPosition(float _x, float _y) {
	position.x = _x;
	position.y = _y;
}

//--------------------------------------------------------------
void RobotState::setGoal(ofVec2f _goal) {
	goal = _goal;
}

//--------------------------------------------------------------
void RobotState::setGoal(float _x, float _y) {
	goal.x = _x;
	goal.y = _y;
}

//--------------------------------------------------------------
void RobotState::setColor(ofColor _color) {
	color = _color;
}

//--------------------------------------------------------------
void RobotState::setColor(uint8_t _r, uint8_t _g, uint8_t _b) {
	color.r = _r;
	color.g = _g;
	color.b = _b;
}

//--------------------------------------------------------------
void RobotState::setId(unsigned int _id) {
	id = _id;
}

//--------------------------------------------------------------
void RobotState::setTouch(uint8_t _touch) {
	touch = _touch;
}

//--------------------------------------------------------------
void RobotState::setOrientation(float _orientation) {
	orientation = _orientation;
}
