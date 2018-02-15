#include "radio.h"

bool isRadioInitialized = false;
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

    if (SPIInit())
    {
        if (initOK = initNRF24L01P())
        {
            if (DEBUG_ENABLED())
                debug_printf("RF successfully initialized, configuring...\n");

            if(id==0)
              initOK &= setChannel(RADIO_CHANNEL);
            else if(id==1)
              initOK &= setChannel(RADIO_CHANNEL+10);
            else if(id==2)
              initOK &= setChannel(RADIO_CHANNEL+20);

            initOK &= setAutoAck(true);
            initOK &= setDataRate(RF24_2MBPS);
            initOK &= enableAckPayload();
            initOK &= setAddressWidth(3);

            if (DEBUG_ENABLED() && initOK)
            {
                debug_printf("RF Configuration OK!\n");
                printDetails();
            }

            openWritingPipe(DEFAULT_ADDRESS);
            openReadingPipe(0, DEFAULT_ADDRESS);

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
    if(msg && size>0 && size <= PAYLOAD_MAX_SIZE)
    {
        lastMessageType = msg->header.type;
        startWrite((uint8_t *)msg, size);
    }
}

/*============================================================================
Name    :   selectRobot
------------------------------------------------------------------------------
Purpose :   opens the writing and reading pipe for the given robot
Input   :   r : pointer to the desired robot 
Output  :   
Return	: 
Notes   :
============================================================================*/
void selectRobot(Robot *r)
{
    int x;
    if (r)
    {
        flush_tx();
        openWritingPipe(r->pipeAddress);
        openReadingPipe(0, r->pipeAddress);
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
