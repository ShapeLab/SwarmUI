#include "radio.h"
#include "led.h"

bool isRadioInitialized = false;
volatile uint32_t communicationWatchdog = 0;

uint64_t myPipeAddress = DEFAULT_ADDRESS;
uint8_t myId = 0;
/*============================================================================
Name    :   initRadio
------------------------------------------------------------------------------
Purpose :   initializes the Radio, with the SPI communication first and then
	    the NRF24L01+ chip
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
bool initRadio()
{
    uint8_t initOK;

    HAL_RCC_MCOConfig(RCC_MCO, RCC_MCOSOURCE_HSE, RCC_MCO_NODIV); //output HSE clock on MCO

    if (SPIInit())
    {
        if (initOK = initNRF24L01P())
        {
            if (DEBUG_ENABLED())
                debug_printf("RF successfully initialized, configuring...\n");

            initOK &= setChannel(RADIO_CHANNEL);
            initOK &= setAutoAck(true);
            initOK &= setDataRate(RF24_2MBPS);
            initOK &= enableAckPayload();

            if (DEBUG_ENABLED() && initOK)
            {
                debug_printf("RF Configuration OK!\n");
                printDetails();
            }

            resetCommunicationWatchdog();
            
            openReadingPipe(0, DEFAULT_ADDRESS);
            openWritingPipe(DEFAULT_ADDRESS);
//            startListening();
            isRadioInitialized = true;
        }
    }
    return initOK;
}

/*============================================================================
Name    :   sendRadioMessage
------------------------------------------------------------------------------
Purpose :   Send a framed message through radio to the current SwarmBots
Input   :    
Output  :   
Return	: 
Notes   :
============================================================================*/
void sendRadioMessage(Message *msg, uint8_t size)
{
    startWrite((uint8_t *)msg, size);
}

/*============================================================================
Name    :   sendAddressRequest
------------------------------------------------------------------------------
Purpose :   sends a message to the given robot to ask for its position
Input   :   robotId : id of the robot to ask for position
Output  :   
Return	:  
Notes   :
============================================================================*/
void sendAddressRequest()
{
    if(DEBUG_ENABLED())
      debug_printf("send request\n");

    setGreenLed(5);

    Message msg;
    msg.header.type = TYPE_NEW_ROBOT;
    msg.header.id = 255;

    openWritingPipe(DEFAULT_ADDRESS);
    openReadingPipe(0, DEFAULT_ADDRESS);

    resetCommunicationWatchdog();

    stopListening();
    sendRadioMessage(&msg, sizeof(Header));
    HAL_Delay(1);
    startListening();

    setGreenLed(0);
}

/*============================================================================
Name    :   resetCommunicationWatchdog
------------------------------------------------------------------------------
Purpose :   resets the communication watchdog counter
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void resetCommunicationWatchdog()
{
    communicationWatchdog = COMMUNICATION_TIMEOUT;
}

/*============================================================================
Name    :   tickCommunicationWatchdog
------------------------------------------------------------------------------
Purpose :   decrements the watchdog counter
Input   :   millis: milliseconds to decrement the watchdog counter
Output  :   
Return	:  
Notes   :
============================================================================*/
void tickCommunicationWatchdog(uint8_t millis)
{
    if(communicationWatchdog >= millis)
      communicationWatchdog -= millis;
    else
      communicationWatchdog = 0;
}

/*============================================================================
Name    :   remainingCommunicationWatchdog
------------------------------------------------------------------------------
Purpose :   gives the amount of time before the end of the watchdog counter
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
uint32_t remainingCommunicationWatchdog()
{
    return communicationWatchdog;
}

/*============================================================================
Name    :   setRobotPipeAddress
------------------------------------------------------------------------------
Purpose :   sets the pipe address to the value in parameter
Input   :   _pipeAddress
Output  :   
Return	:  
Notes   :
============================================================================*/
void setRobotPipeAddress(uint64_t _pipeAddress)
{
    myPipeAddress = _pipeAddress;
}

/*============================================================================
Name    :   setRobotId
------------------------------------------------------------------------------
Purpose :  sets the id to the value in parameter
Input   :   _id
Output  :   
Return	:  
Notes   :
============================================================================*/
void setRobotId(uint8_t _id)
{
      myId = _id;
}

/*============================================================================
Name    :   getRobotPipeAddress
------------------------------------------------------------------------------
Purpose :   return the current pipe address
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
uint64_t getRobotPipeAddress()
{
    return myPipeAddress;
}

/*============================================================================
Name    :   getRobotId
------------------------------------------------------------------------------
Purpose :  return the current Id
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
uint8_t getRobotId()
{
      return myId;
}

/*============================================================================
Name    :   openCommunication
------------------------------------------------------------------------------
Purpose :   opens the writing and reading pipe for the given robot
Input   :   
Output  :   
Return	: 
Notes   :
============================================================================*/
void openCommunication()
{
    openWritingPipe(myPipeAddress);
    openReadingPipe(0, myPipeAddress);
}