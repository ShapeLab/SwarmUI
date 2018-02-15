//
//  ZooidReceiver.hpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 28/01/16.
//
//

#ifndef SwarmReceiver_hpp
#define SwarmReceiver_hpp

#include "ofMain.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include "ofSerial.h"
#include "Message.h"
#include "configuration.h"

#define MINIMUM_BYTES_TO_READ 5
#define NB_MAX_VALUES 20
#define HANDSHAKE_REQUEST "Are you?"
#define HANDSHAKE_REPLY "You are?"
#define HANDSHAKE_LEAVE "Bye!"

#define TYPE_UPDATE                             0x03
#define TYPE_STATUS                             0x04
#define TYPE_MOTORS_VELOCITY                    0x05
#define TYPE_ROBOT_POSITION                     0x06
#define TYPE_RECEIVER_INFO                      0x10
#define TYPE_RECEIVER_CONFIG                    0x11
#define TYPE_HANDSHAKE_REQUEST                  0x12
#define TYPE_HANDSHAKE_REPLY                    0x13
#define TYPE_HANDSHAKE_LEAVE                    0x14

#define RECEIVER_RECIPIENT                      250
#define MANAGER_ID                              251


typedef struct {
	uint16_t positionX;
	uint16_t positionY;
	uint8_t colorRed;
	uint8_t colorGreen;
	uint8_t colorBlue;
	uint8_t preferredSpeed;
	int16_t orientation;
	bool isFinalGoal;
	uint8_t empty;
}PositionControlMessage;

typedef struct {
	uint16_t positionX;
	uint16_t positionY;
	int16_t orientation;
	uint8_t state;
	uint8_t batteryLevel;
}StatusMessage;

typedef struct {
    uint8_t receiverId;
    uint8_t numZooids;
    uint8_t updateFrequency;
    uint8_t empty;
}ReceiverConfigMessage;


class ZooidReceiver
{
private:
    int receiverId;
	ofSerial serialPort;
	mutex dataInMutex, valuesMutex, dataOutMutex;
	condition_variable sendingCond, processCond;
    vector<uint8_t> bufferIn;
    vector<uint8_t> bufferOut;
    vector<Message> incomingMessages; 
	thread processingThread;
    thread receivingThread;
    thread sendingThread;
    unsigned int bytesToSend;
    bool dataReceived;
	bool threadsRunning;
    bool readyToSend;
    bool initialized;

    void usbReceivingRoutine();
    void processIncomingData();
    void usbSendingRoutine();
    
public:
    ZooidReceiver();
    ZooidReceiver(unsigned int id);
    ZooidReceiver(string descriptor);
    ~ZooidReceiver();

    bool init();
    bool init(string descriptor);
	bool connect(int deviceId, int baudrate);
    bool connect(string description, int baudrate);
	bool isInitialized();
    void addMessage(Message m);
    Message getLastMessage();
    unsigned int availableMessages();
    bool isDataReadyToSend();
    int getId();

    uint8_t* getLastData();
    void reset();
    void disconnect();
	void sendUSB(uint8_t type, uint8_t dest, uint8_t length, uint8_t* data, bool sendNow = false); 
    void setReadyToSend();
};
#endif /* SwarmReceiver_hpp */
