#include "radio.h"
#include "led.h"

bool isRadioInitialized = false;

uint64_t myPipeAddress = DEFAULT_ADDRESS;
uint8_t myId = 0;

uint8_t lastMessageType = 0;

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
bool initRadio(uint8_t id)
{
    uint8_t initOK;

    HAL_RCC_MCOConfig(RCC_MCO, RCC_MCOSOURCE_HSE, RCC_MCO_NODIV); //output HSE clock on MCO

    if (SPIInit())
    {
        if (initOK = initNRF24L01P())
        {
            if (DEBUG_ENABLED())
                debug_printf("RF successfully initialized, configuring...\n");

            if(id<10)
              initOK &= setChannel(RADIO_CHANNEL);
            else if(id>=10 && id<20)
              initOK &= setChannel(RADIO_CHANNEL+10);
            else if(id>=20)
              initOK &= setChannel(RADIO_CHANNEL+20);

            initOK &= setChannel(RADIO_CHANNEL);// + (id / NB_MAX_ROBOTS) * NB_MAX_ROBOTS);
            initOK &= setAutoAck(true);
            initOK &= setDataRate(RF24_2MBPS);
            initOK &= enableAckPayload();
            initOK &= setAddressWidth(3);

            if (DEBUG_ENABLED() && initOK)
            {
                debug_printf("RF Configuration OK!\n");
                printDetails();
            }

            setRobotId(id);
            setRobotPipeAddress(DEFAULT_ADDRESS + 12 + getRobotId() * 3);

            openCommunication();
            startListening();
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
    if (msg && size > 0)
    {
        lastMessageType = msg->header.type;
        startWrite((uint8_t *)msg, size);
    }
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

/*============================================================================
Name    :   prepareAnswer
------------------------------------------------------------------------------
Purpose :   Stacks the answer to be sent as soon as a message arrives from 
            the receiver
Input   :   *msg : pointer to message to be sent
            size : size of the message to be sent
Output  :   
Return	: 
Notes   :
============================================================================*/
void prepareAnswer(Message *msg, uint8_t size)
{
    if (msg && size > 0)
    {
        lastMessageType = msg->header.type;
        writeAckPayload(0, (uint8_t *)msg, size);
    }
}

/*============================================================================
Name    :   getLastMessageType
------------------------------------------------------------------------------
Purpose :   returns the type of the last message sent
Input   :   
Output  :   
Return	: 
Notes   :
============================================================================*/
uint8_t getLastMessageType()
{
    return lastMessageType;
}