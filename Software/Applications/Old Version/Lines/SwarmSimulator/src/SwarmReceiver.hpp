//
//  SwarmReceiver.hpp
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
#include "Message.hpp"

#define MINIMUM_BYTES_TO_READ 5
#define NB_MAX_VALUES 100
#define RECEPTION_TIMEOUT 1000

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
	uint8_t touch;
	uint8_t batteryLevel;
}StatusMessage;



class SwarmReceiver
{
private:
    ofSerial serialPort;
	timed_mutex COMPortMutex, dataInMutex, valuesMutex, dataOutMutex;
    vector<uint8_t> bufferIn;
    vector<uint8_t> bufferOut;
    vector<Message> incomingMessages; 
	thread processingThread;
    thread receivingThread;
    thread sendingThread;
    uint8_t bytesToSend;
	string comPort;

	uint64_t lastReceptionTimestamp;

    bool dataReceived;
    void setNewDataEvent();
    void waitNewDataEvent();
    
    void usbReceivingRoutine();
    void processIncomingData();
    void usbSendingRoutine();

	bool firstReception;


public:
	SwarmReceiver(int id);
    SwarmReceiver(string desc);

	~SwarmReceiver();
	bool connect(int deviceId, int baudrate);
    bool connect(string description, int baudrate);
	bool isInitialized();
    void addMessage(Message m);
    Message getLastMessage();
    unsigned int availableMessages();
    bool isDataReadyToSend();
    uint8_t* getLastData();
    void reset();
    void disconnect();
    void sendUSB(uint8_t type, uint8_t dest, uint8_t length, uint8_t* data);
};
#endif /* SwarmReceiver_hpp */
