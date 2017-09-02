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
    receiverId = -1;
    threadsRunning = true;
    readyToSend = false;
    
    bytesToSend = 0;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
}

//--------------------------------------------------------------
ZooidReceiver::ZooidReceiver(string descriptor) {
    receiverId = -1;
    threadsRunning = true;
    readyToSend = false;
    
    bytesToSend = 0;
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
    return serialPort.isInitialized();
}

//--------------------------------------------------------------
bool ZooidReceiver::connect(int deviceId, int baudrate) {
    if (serialPort.setup(deviceId, baudrate)) {
        serialPort.writeBytes((unsigned char*)HANDSHAKE_REQUEST, sizeof(HANDSHAKE_REQUEST));
        
        long timeOut = ofGetElapsedTimeMillis();
        while((serialPort.available() <  sizeof(HANDSHAKE_REPLY)) || (ofGetElapsedTimeMillis() - timeOut > 1000));
        
        if(timeOut > 1000) return false;
        
        uint8_t *buffer = new uint8_t[serialPort.available()];
        serialPort.readBytes(buffer, serialPort.available());
        
        if(strcmp((char*)buffer, (char*)HANDSHAKE_REPLY) == 0) {
            receivingThread = thread(&ZooidReceiver::usbReceivingRoutine, this);
            processingThread = thread(&ZooidReceiver::processIncomingData, this);
            sendingThread = thread(&ZooidReceiver::usbSendingRoutine, this);
            
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
bool ZooidReceiver::connect(string description, int baudrate) {
    if (serialPort.setup(description, baudrate)) {
        serialPort.writeBytes((unsigned char*)HANDSHAKE_REQUEST, sizeof(HANDSHAKE_REQUEST));
    
        long timeOut = ofGetElapsedTimeMillis();
        while((serialPort.available() <  sizeof(HANDSHAKE_REPLY))){
            long t = ofGetElapsedTimeMillis() - timeOut;
            if(t > 1000){
                serialPort.close();
                return false;
            }
        }
        
        uint8_t *buffer = new uint8_t[serialPort.available()];
        serialPort.readBytes(buffer, serialPort.available());
        
        if(strcmp((char*)buffer, (char*)HANDSHAKE_REPLY) == 0) {
            receivingThread = thread(&ZooidReceiver::usbReceivingRoutine, this);
            processingThread = thread(&ZooidReceiver::processIncomingData, this);
            sendingThread = thread(&ZooidReceiver::usbSendingRoutine, this);
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
void ZooidReceiver::usbReceivingRoutine() {
    uint16_t bytesToRead = 0;
    
    while (threadsRunning) {
#ifdef TARGET_WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
        if (serialPort.isInitialized()) {
            bytesToRead = (uint16_t) serialPort.available();
            if (bytesToRead > MINIMUM_BYTES_TO_READ) {
                uint8_t *readData = new uint8_t[bytesToRead];
                if (serialPort.readBytes(readData, bytesToRead) == bytesToRead) {
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
        if (serialPort.isInitialized()){// && readyToSend) {
            unique_lock<mutex> lock(dataOutMutex);
            sendingCond.wait(lock, [this]() { return (bytesToSend > 0) | !threadsRunning; });
            if (bytesToSend>0) {
                if(bytesToSend < 63) { //USB FS can only send 64 bytes at a time
                    if(serialPort.writeBytes(bufferOut.data(), bytesToSend) != OF_SERIAL_ERROR) {
                        bufferOut.erase(bufferOut.begin(), bufferOut.begin() + bytesToSend);
                        bytesToSend = 0;
                        readyToSend = false;
                    }
                } else {
                    if(serialPort.writeBytes(bufferOut.data(), 63) != OF_SERIAL_ERROR) {
                        if(bufferOut.size()<63)
                            cout<<"pb"<<endl;
                        else
                            bufferOut.erase(bufferOut.begin(), bufferOut.begin() + 63);
                        bytesToSend -= 63;
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
void ZooidReceiver::sendUSB(uint8_t type, uint8_t dest, uint8_t length, uint8_t *data) {
    
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
    unique_lock<mutex> lock(valuesMutex);
    Message msg = incomingMessages[incomingMessages.size() - 1];
    incomingMessages.pop_back();
    lock.unlock();
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
    threadsRunning = false;
    
    receivingThread.join();
    processCond.notify_one();
    processingThread.join();
    sendingCond.notify_one();
    sendingThread.join();
    
    serialPort.writeBytes((unsigned char*)HANDSHAKE_LEAVE, sizeof(HANDSHAKE_LEAVE));
#ifdef TARGET_WIN32
    Sleep(100);
#else
    usleep(100000);
#endif
    
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

//--------------------------------------------------------------
vector<string> ZooidReceiver::getAvailableZooidReceivers() {
    vector<string> descriptors;
    ofSerial port;
	vector<ofSerialDeviceInfo> devices = port.getDeviceList();
    
    for(int i=0;i<devices.size();i++) {
		string currentDevice = devices[i].getDeviceName();
#ifdef TARGET_WIN32
        if(currentDevice.find("COM") != -1)
			descriptors.push_back(currentDevice.substr(currentDevice.find('(') + 1,
								  currentDevice.length() - (currentDevice.find('(') + 1)));
#else
        if(currentDevice.find("tty.usbmodem") != -1)
            descriptors.push_back(currentDevice);
#endif
        
    }
    return descriptors;
}
