//
//  SwarmReceiver.cpp
//  SwarmSimulator
//
//  Created by Mathieu Le Goc on 28/01/16.
//
//

#include "SwarmReceiver.hpp"


SwarmReceiver::SwarmReceiver(int id)
{
	firstReception = true;

    bytesToSend = 0;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
    
    vector <ofSerialDeviceInfo> devices = serialPort.getDeviceList();
    string desc = "";

#ifdef TARGET_WIN32
    if(id == 0) connect("COM3", 115200);
    //if (id == 0) connect("COM9", 115200);
    
    //else if(id == 1) connect("COM3", 115200);
    //else if (id == 2) connect("COM10", 115200);
#else
    for(int i=0;i<devices.size();i++){
		if (devices[i].getDeviceName().find("tty.usbmodem") != -1) {
            if(connect(devices[i].getDevicePath(), 115200)){
                break;
            }
        }
    }
#endif
}

SwarmReceiver::SwarmReceiver(string desc)
{
    firstReception = true;
    
    bytesToSend = 0;
    bufferIn = vector<uint8_t>();
    bufferOut = vector<uint8_t>();
    incomingMessages = vector<Message>();
    
    vector <ofSerialDeviceInfo> devices = serialPort.getDeviceList();

    connect(desc, 115200);
}


SwarmReceiver::~SwarmReceiver()
{
	bufferIn.clear();
	bufferOut.clear();
}


bool SwarmReceiver::isInitialized()
{
    return serialPort.isInitialized();
}

bool SwarmReceiver::connect(int deviceId, int baudrate)
{
	if (serialPort.setup(deviceId, baudrate))
	{
		comPort = serialPort.getDeviceList()[deviceId].getDeviceName();
		receivingThread = thread(&SwarmReceiver::usbReceivingRoutine, this);
		processingThread = thread(&SwarmReceiver::processIncomingData, this);
		sendingThread = thread(&SwarmReceiver::usbSendingRoutine, this);
	}
	return serialPort.isInitialized();
}

bool SwarmReceiver::connect(string description, int baudrate)
{
    
    if(serialPort.setup(description, baudrate))
    {
		comPort = description;
        receivingThread = thread(&SwarmReceiver::usbReceivingRoutine, this);
        processingThread = thread(&SwarmReceiver::processIncomingData, this);
        sendingThread = thread(&SwarmReceiver::usbSendingRoutine, this);
    }
    return serialPort.isInitialized();
}

void SwarmReceiver::usbReceivingRoutine()
{
	lastReceptionTimestamp = ofGetElapsedTimeMillis();

    uint16_t bytesToRead = 0;
    
    while (true)
    {
#ifdef TARGET_WIN32
        Sleep(0);
#else
        usleep(10);
#endif
        if (serialPort.isInitialized() == true)
        {
			if(firstReception)
				lastReceptionTimestamp = ofGetElapsedTimeMillis();
				
			int t = ofGetElapsedTimeMillis() - lastReceptionTimestamp;
			if (t > RECEPTION_TIMEOUT && !firstReception)
				reset();
		
			else
			{
				COMPortMutex.lock();
				bytesToRead = serialPort.available();
				if (bytesToRead > MINIMUM_BYTES_TO_READ)
				{
					if (firstReception)
						firstReception = false;

					lastReceptionTimestamp = ofGetElapsedTimeMillis();

					uint8_t *readData = new uint8_t[bytesToRead];

					if (serialPort.readBytes(readData, bytesToRead) == bytesToRead)
					{
						//cout << "receive in" << endl;
						dataInMutex.lock();
						bufferIn.insert(bufferIn.end(), readData, readData + bytesToRead);
						setNewDataEvent();
						dataInMutex.unlock();
						//cout << "receive out" << endl;
					}
				}
				COMPortMutex.unlock();
			}
        }
    }
}

void SwarmReceiver::usbSendingRoutine()
{
    while(true)
    {
#ifdef TARGET_WIN32
        Sleep(0);
#else
        usleep(10);
#endif
		if (bytesToSend > 0 && serialPort.isInitialized() == true)
		{
			if (bytesToSend < 64)//USB FS can only send 64 bytes at a time
			{
				//cout << "send in" << endl;
				COMPortMutex.lock();	
				dataOutMutex.lock();
					serialPort.writeBytes(bufferOut.data(), bytesToSend);
					bufferOut.erase(bufferOut.begin(), bufferOut.begin() + bytesToSend); 		
					bytesToSend = 0;
				dataOutMutex.unlock();				
				COMPortMutex.unlock();
				//cout << "send out" << endl;
			}
			else
			{
				//cout << "send in" << endl;
				COMPortMutex.lock();
				dataOutMutex.lock();
					serialPort.writeBytes(bufferOut.data(), 64);
					bufferOut.erase(bufferOut.begin(), bufferOut.begin() + 64);
					bytesToSend -= 64;
				dataOutMutex.unlock();			
				COMPortMutex.unlock();
				//cout << "send out" << endl;
			}
        }
    }
}

void SwarmReceiver::processIncomingData()
{
    while (true)
    {
#ifdef TARGET_WIN32
        Sleep(0);
#else
        usleep(10);
#endif
        if (bufferIn.size() >= 4)   //leave some space to be able to read the payload size uint8_t
        {
            dataInMutex.lock();
            
            if (bufferIn[0] == '~')  //if he finds the starting character
            {
                uint8_t messageType = bufferIn[1];
                uint8_t sourceAddr = bufferIn[2];
                uint8_t payloadSize = bufferIn[3];
                
                if ((payloadSize + 4) < bufferIn.size()) //checks if the message can fit in the buffer
                {
                    if (bufferIn[payloadSize + 4] == '!') //checks that the ending character is there
                    {
                        uint8_t* payload = new uint8_t[payloadSize];
                        
                        for (int j = 0; j < payloadSize; j++)
                            payload[j] = bufferIn[j + 4];
                        
                        bufferIn.erase(bufferIn.begin(), bufferIn.begin()+payloadSize+5);
                        
                        dataInMutex.unlock();
                        
                        addMessage(Message(sourceAddr, messageType, payload));
                    }
                    else
                    {
                        bufferIn.erase(bufferIn.begin());
                        dataInMutex.unlock();
                    }
                }
                else
                {
                    dataInMutex.unlock();
                    waitNewDataEvent();
                }
            }
            else
            {
                bufferIn.erase(bufferIn.begin());
                dataInMutex.unlock();
            }
        }
        else
        {
            waitNewDataEvent();
        }
    }
}

void SwarmReceiver::setNewDataEvent()
{
    dataReceived = true;
}

void SwarmReceiver::waitNewDataEvent()
{
    dataReceived = false;
    while(!dataReceived)
    {
#ifdef TARGET_WIN32
        Sleep(0);
#else
        usleep(10);
#endif
    }
}

void SwarmReceiver::addMessage(Message m)
{
    valuesMutex.lock();
    ///  handle the different types of messages
    if (incomingMessages.size() >= NB_MAX_VALUES)
        incomingMessages.erase(incomingMessages.begin());
    incomingMessages.push_back(m);
    valuesMutex.unlock();
}

Message SwarmReceiver::getLastMessage()
{
    valuesMutex.lock();
    Message msg = incomingMessages[incomingMessages.size() - 1];
    incomingMessages.pop_back();
    valuesMutex.unlock();
    return msg;
}

unsigned int SwarmReceiver::availableMessages()
{
    return incomingMessages.size();
}

bool SwarmReceiver::isDataReadyToSend()
{
    if (incomingMessages.size() > 0)
        return true;
    else
        return false;
}

uint8_t* SwarmReceiver::getLastData()
{
    if (incomingMessages.size() > 0)
        return getLastMessage().ToByteArray();
    else
        return NULL;
}

void SwarmReceiver::reset()
{
	uint8_t resetMsg[] = { '~', 0xff, 250, 0, '!' };

	COMPortMutex.lock();
	serialPort.writeBytes(resetMsg, sizeof(resetMsg));
	//serialPort.flush();
	//serialPort.close();
	firstReception = true;
	cout << "Resetting " << comPort << endl;
    dataInMutex.lock();
    bufferIn.clear();
    dataInMutex.unlock();
    
    valuesMutex.lock();
    incomingMessages.clear();
    valuesMutex.unlock();
	
	do
	{
	#ifdef TARGET_WIN32
		Sleep(1000);
	#else
		usleep(1000000);
	#endif
	} while (!serialPort.setup(comPort, 115200));

	COMPortMutex.unlock();
}

void SwarmReceiver::disconnect()
{
    bufferIn.clear();
    incomingMessages.clear();
    
    serialPort.close();
}


void SwarmReceiver::sendUSB(uint8_t type, uint8_t dest, uint8_t length, uint8_t* data)
{
	static uint16_t i = 0;
    if (length > 0 && data != NULL)
    {
        dataOutMutex.lock();

		if (bufferOut.size() > 1024)
		{
			bufferOut.clear();
			bytesToSend = 0;
		}

        bufferOut.push_back('~');	
        bufferOut.push_back(type);
		bufferOut.push_back(dest);
        bufferOut.push_back(length);
        bufferOut.insert(bufferOut.end(), data, data+length);
        bufferOut.push_back('!');
        
        dataOutMutex.unlock();
        bytesToSend += (length + 5);

    }
}



