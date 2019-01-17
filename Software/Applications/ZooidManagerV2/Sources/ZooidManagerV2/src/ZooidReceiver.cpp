//
//  ZooidReceiver.cpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 28/01/16.
//
//

#include "ZooidReceiver.h"

#ifndef TARGET_WIN32

#include <unistd.h>
#include <fcntl.h>

#endif

//--------------------------------------------------------------
ZooidReceiver::ZooidReceiver() {
    receiverId = 0;
    threadsRunning = true;
    readyToSend = false;
    
    bytesToSend = 0;
    initialized = false;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
}

//--------------------------------------------------------------
ZooidReceiver::ZooidReceiver(unsigned int id) {
    receiverId = id;
    threadsRunning = true;
    readyToSend = false;
    
    bytesToSend = 0;
    initialized = false;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
}

//--------------------------------------------------------------
ZooidReceiver::ZooidReceiver(string descriptor) {
    receiverId = 0;
    threadsRunning = true;
    readyToSend = false;
    
    bytesToSend = 0;
    initialized = false;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
    init(descriptor);
}

//--------------------------------------------------------------
ZooidReceiver::~ZooidReceiver() {
    if(isInitialized()) {
        disconnect();
    }
}

//--------------------------------------------------------------
bool ZooidReceiver::init() {
    vector<ofSerialDeviceInfo> devices = serialPort.getDeviceList();
    for (int i = 0; i < devices.size(); i++) {
#ifdef TARGET_WIN32
        return connect(devices[i].getDevicePath(), 115200);
#else
        if (devices[i].getDeviceName().find("tty.usbmodem") != -1) {
            return connect(devices[i].getDevicePath(), 115200);
        }
#endif
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidReceiver::init(string descriptor) {
    return connect(descriptor, 115200);
}

//--------------------------------------------------------------
bool ZooidReceiver::isInitialized() {
    return initialized;
}

//--------------------------------------------------------------
bool ZooidReceiver::connect(int deviceId, int baudrate) {
    if (serialPort.setup(deviceId, baudrate)) {
        initialized = true;

        receivingThread = thread(&ZooidReceiver::usbReceivingRoutine, this);
        processingThread = thread(&ZooidReceiver::processIncomingData, this);
        sendingThread = thread(&ZooidReceiver::usbSendingRoutine, this);
        
        sendUSB(TYPE_HANDSHAKE_REQUEST, RECEIVER_RECIPIENT, sizeof(HANDSHAKE_REQUEST), (unsigned char*)HANDSHAKE_REQUEST, true);
        
        long timeOut = ofGetElapsedTimeMillis();
        Message lastMessage;
        
        while (lastMessage.getSenderId() != MANAGER_ID && lastMessage.getType() != TYPE_HANDSHAKE_REPLY){
            ofSleepMillis(1);
            
            lastMessage = getLastMessage();
            long t = ofGetElapsedTimeMillis() - timeOut;
            if(t > 1000){
                disconnect();
                return false;
            }
        }
        
        if(lastMessage.getPayload() && string((char*)lastMessage.getPayload()).compare(HANDSHAKE_REPLY) == 0){
            ReceiverConfigMessage config;
            config.receiverId = receiverId;
            config.numZooids = NUM_ZOOIDS_PER_RECEIVER;
            config.updateFrequency = int(SYSTEM_UPDATE_FREQUENCY);
            sendUSB(TYPE_RECEIVER_CONFIG, RECEIVER_RECIPIENT, sizeof(config), (uint8_t*)&config, true);
            
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidReceiver::connect(string description, int baudrate) {
    if (serialPort.setup(description, baudrate)) {
        initialized = true;

        cout << "Connecting to " << description << endl;
        receivingThread = thread(&ZooidReceiver::usbReceivingRoutine, this);
        processingThread = thread(&ZooidReceiver::processIncomingData, this);
        sendingThread = thread(&ZooidReceiver::usbSendingRoutine, this);
        
        sendUSB(TYPE_HANDSHAKE_REQUEST, RECEIVER_RECIPIENT, sizeof(HANDSHAKE_REQUEST), (unsigned char*)HANDSHAKE_REQUEST, true);
        setReadyToSend();
        
        long timeOut = ofGetElapsedTimeMillis();
        Message lastMessage;
        
        while (lastMessage.getType() != TYPE_HANDSHAKE_REPLY) {
            ofSleepMillis(1);
            
            lastMessage = getLastMessage();
            long t = ofGetElapsedTimeMillis() - timeOut;
            if(t > 1000){
                disconnect();
                return false;
            }
        }
        
        if(lastMessage.getPayload() && string((char*)lastMessage.getPayload()).compare(HANDSHAKE_REPLY) == 0){
            ReceiverConfigMessage config;
            config.receiverId = receiverId;
            config.numZooids = NUM_ZOOIDS_PER_RECEIVER;
            config.updateFrequency = int(SYSTEM_UPDATE_FREQUENCY);
            sendUSB(TYPE_RECEIVER_CONFIG, RECEIVER_RECIPIENT, sizeof(config), (uint8_t*)&config, true);
            setReadyToSend();
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
void ZooidReceiver::usbReceivingRoutine() {
    uint16_t bytesToRead = 0;
    
    while (threadsRunning) {
        ofSleepMillis(1);
        if (initialized && serialPort.isInitialized()) {
            bytesToRead = (uint16_t) serialPort.available();
            if (bytesToRead > MINIMUM_BYTES_TO_READ) {
                uint8_t *readData = new uint8_t[bytesToRead];
                if (serialPort.readBytes(readData, bytesToRead)>0) {// == bytesToRead) {
                    unique_lock<mutex> lock(dataInMutex);
                    bufferIn.insert(bufferIn.end(), readData, readData + bytesToRead);
                    lock.unlock();
                    processCond.notify_one();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ZooidReceiver::processIncomingData() {
    while (threadsRunning) {
        unique_lock<mutex> lock(dataInMutex);
        processCond.wait(lock, [this]() { return (bufferIn.size() >= 4) | !threadsRunning;}); //leave some space to be able to read the payload size uint8_t
        
        if(bufferIn.size() >= 4) {
            if (bufferIn[0] == '~') {  //if he finds the starting character
                uint8_t messageType = bufferIn[1];
                uint8_t sourceAddr = bufferIn[2];
                uint8_t payloadSize = bufferIn[3];
                
                if ((payloadSize + 4) < bufferIn.size()) { //checks if the message can fit in the buffer
                    if (bufferIn[payloadSize + 4] == '!') { //checks that the ending character is there
                        switch (messageType) {
                            case TYPE_RECEIVER_INFO:
                                receiverId = sourceAddr;
                                cout<<"ZooidReceiver #"<<receiverId<<" succesfully connected"<<endl;
                                break;
                            default:
                                uint8_t *payload = new uint8_t[payloadSize];
                                for (int j = 0; j < payloadSize; j++) {
                                    payload[j] = bufferIn[j + 4];
                                }
                                addMessage(Message(sourceAddr, messageType, payload));
                                break;
                        }
                        bufferIn.erase(bufferIn.begin(), bufferIn.begin() + payloadSize + 5);
                        
                    } else {
                        bufferIn.erase(bufferIn.begin());
                    }
                }
            }
            else {
                bufferIn.erase(bufferIn.begin());
            }
        }
        lock.unlock();
    }
}

//--------------------------------------------------------------
void ZooidReceiver::usbSendingRoutine() {
    while (threadsRunning) {
        if (initialized && serialPort.isInitialized()){// && readyToSend) {
            unique_lock<mutex> lock(dataOutMutex);
            sendingCond.wait(lock, [this]() { return (bytesToSend > 0) | !threadsRunning; });
            if (bytesToSend>0) {
                if(bytesToSend < 63) { //USB FS can only send 64 bytes at a time
                    try{
                        if(serialPort.writeBytes(bufferOut.data(), bytesToSend) != OF_SERIAL_ERROR) {
                            bufferOut.erase(bufferOut.begin(), bufferOut.begin() + bytesToSend);
                            bytesToSend = 0;
                            readyToSend = false;
                        }
                    }catch(int e){
                        initialized = false;
                    }
                    
                }
                else {
                    try{
                        if(serialPort.writeBytes(bufferOut.data(), 63) != OF_SERIAL_ERROR) {
                            bufferOut.erase(bufferOut.begin(), bufferOut.begin() + 63);
                            bytesToSend -= 63;
                        }
                    }catch(int e){
                        initialized = false;
                    }
                }
            }
            lock.unlock();
        }
    }
}

//--------------------------------------------------------------
void ZooidReceiver::setReadyToSend(){
    sendingCond.notify_one();
}

//--------------------------------------------------------------
void ZooidReceiver::sendUSB(uint8_t type, uint8_t dest, uint8_t length, uint8_t *data, bool sendNow) {
    
    if (threadsRunning && length > 0 && data != NULL) {
        unique_lock<mutex> lock(dataOutMutex);
        if (bufferOut.size() > 1024) {
            bufferOut.erase(bufferOut.begin(), bufferOut.begin() + length + 5);
            //bytesToSend = 0;
        }
        
        bufferOut.push_back('~');
        bufferOut.push_back(type);
        bufferOut.push_back(dest);
        
        bufferOut.push_back(length);
        bufferOut.insert(bufferOut.end(), data, data + length);
        bufferOut.push_back('!');
        
        bytesToSend += length + 5;
        lock.unlock();
        
        if (sendNow)
            setReadyToSend();
    }
}

//--------------------------------------------------------------
void ZooidReceiver::addMessage(Message m) {
    unique_lock<mutex> lock(valuesMutex);
    //  handle the different types of messages
    if (incomingMessages.size() >= NB_MAX_VALUES) {
        incomingMessages.erase(incomingMessages.begin());
    }
    incomingMessages.push_back(m);
    lock.unlock();
}

//--------------------------------------------------------------
Message ZooidReceiver::getLastMessage() {
    Message msg;
    if(incomingMessages.size()>0) {
        unique_lock<mutex> lock(valuesMutex);
        msg = incomingMessages.back();
        incomingMessages.pop_back();
        lock.unlock();
    }
    return msg;
}

//--------------------------------------------------------------
int ZooidReceiver::getId() {
    return receiverId;
}

//--------------------------------------------------------------
unsigned int ZooidReceiver::availableMessages() {
    return (unsigned int)incomingMessages.size();
}

//--------------------------------------------------------------
bool ZooidReceiver::isDataReadyToSend() {
    return incomingMessages.size() != 0;
}

//--------------------------------------------------------------
uint8_t *ZooidReceiver::getLastData() {
    if (incomingMessages.size() > 0) {
        return getLastMessage().ToByteArray();
    } else {
        return NULL;
    }
}

//--------------------------------------------------------------
void ZooidReceiver::reset() {
    serialPort.flush();
    unique_lock<mutex> lock(dataInMutex);
    bufferIn.clear();
    lock.unlock();
    
    unique_lock<mutex> lock2(valuesMutex);
    incomingMessages.clear();
    lock2.unlock();
}

//--------------------------------------------------------------
void ZooidReceiver::disconnect() {
    
    if(serialPort.isInitialized() && initialized ){
        initialized = false;
        sendUSB(TYPE_HANDSHAKE_LEAVE, RECEIVER_RECIPIENT, sizeof(HANDSHAKE_LEAVE), (unsigned char*)HANDSHAKE_LEAVE, true);
        
        while(bufferOut.size() > 0){
            ofSleepMillis(1);
        }
    }
    
    threadsRunning = false;
    
    receivingThread.join();
    processCond.notify_one();
    processingThread.join();
    sendingCond.notify_one();
    sendingThread.join();
    
    unique_lock<mutex> lock(dataInMutex);
    bufferIn.clear();
    lock.unlock();
    
    unique_lock<mutex> lock1(dataOutMutex);
    bufferOut.clear();
    lock1.unlock();
    
    unique_lock<mutex> lock2(valuesMutex);
    incomingMessages.clear();
    lock2.unlock();
    
    incomingMessages.clear();
    serialPort.close();
}

