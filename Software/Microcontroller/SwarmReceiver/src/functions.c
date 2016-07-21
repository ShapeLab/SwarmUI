#include "functions.h"
#include "USBD_CDC.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

RobotRingBuffer myRobots;
Robot newComingRobot;
uint8_t robotToPoll = 0;
bool canPollNextRobot = true;
volatile bool radioEvent = false;
volatile bool usbDataAvailable = false; //modified in stn32f0xx_it.c

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*============================================================================
Name    :   initSystem
------------------------------------------------------------------------------
Purpose :   initializes the entire system
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void initReceiver()
{
    HAL_Init();
    /* Configure the system clock to have a system clock = 48 Mhz */
    SystemClock_Config();

    initLeds();
    initUSB();
    if (!initRadio())
        Error_Handler();

    RobotRingBuffer_init(&myRobots);
    //    prepareNewRobot(&newComingRobot);

    // add 5 robots
//    for (int i = 0; i < 5; i++)
//    {
//        newComingRobot.pipeAddress = DEFAULT_ADDRESS + i * 2;
//        newComingRobot.id = i;
//        RobotRingBuffer_push(&myRobots, newComingRobot);
//    }
}

/*============================================================================
Name    :   checkRadio
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from the radio
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void checkRadio()
{
    static uint32_t timeSent = 0;
    uint32_t currentTime = HAL_GetTick();

    if(robotToPoll == 0 && myRobots.count > 0)
      stopListening();

    if (robotToPoll < myRobots.count)
    {
        if (canPollNextRobot)
        {
            setRedLed(255);
            sendRobotUpdate(robotToPoll);
            canPollNextRobot = false;
            timeSent = currentTime;
        }
        else
        {
            if (currentTime - timeSent > 2) //timeout of 3ms
            {
                tickRobotWatchdogTimer(robotToPoll);
                flush_tx();
                setRedLed(0);
                canPollNextRobot = true;
                robotToPoll++;
            }
        }
    }
    else 
      startListening();

    if (radioEvent)
    {
        bool tx, fail, rx;
        whatHappened(&tx, &fail, &rx);

        if (rx)
        {
            handleIncomingRadioMessage();
            if (isRxFifoEMpty())
            {
                clearInterruptFlag(true, false, false);
                radioEvent = false;
            }
        }
        if (fail)
        {
            flush_tx();
            clearInterruptFlag(false, false, true);
            radioEvent = false;
        }
        if (tx)
        {
            clearInterruptFlag(false, true, false);
            radioEvent = false;
        }
    }
}

/*============================================================================
Name    :   handleIncomingRadioMessage
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from the radio
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void handleIncomingRadioMessage()
{
    uint8_t payloadSize = getDynamicPayloadSize();
    if (payloadSize > PAYLOAD_MAX_SIZE)
        flush_rx();
    else if(payloadSize>0)
    {
        Message msg;
        memset(&msg, 0, sizeof(msg));
        readRadio((uint8_t *)&msg, payloadSize);

        switch (msg.header.type)
        {
        case TYPE_STATUS:
            blinkStatusLed();
            resetRobotWatchdogTimer(robotToPoll);
            writeUSB(&msg, payloadSize - sizeof(Header));
            robotToPoll++;
            canPollNextRobot = true;
            setRedLed(0);
            break;

        case TYPE_NEW_ROBOT:
            assignNewRobot();
            break;

        default:
            break;
        }
    }
}

/*============================================================================
Name    :   sendRobotUpdate
------------------------------------------------------------------------------
Purpose :   sends a message to the given robot with the motors speeds and
            to ask for its position
Input   :   robotId : id of the robot to ask for position
Output  :   
Return	:  
Notes   :
============================================================================*/
void sendRobotUpdate(uint8_t robotId)
{
    if (myRobots.count > 0 && robotId < myRobots.count)
    {
        Robot *r = RobotRingBuffer_get(&myRobots, robotId);
        if (r)
        {
            if(r->watchdogTimer <= MAX_MESSAGES_LOST)
            {
                selectRobot(r);
                Message msg;
                msg.header.type = TYPE_UPDATE;
                msg.header.id = RECEIVER_ID;
                memcpy_fast(msg.payload, r->dataBuffer, r->payloadSize);
                sendRadioMessage(&msg, r->payloadSize + sizeof(Header));
            }
        }
    }
}

/*============================================================================
Name    :   checkUSB
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from the USB
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void checkUSB()
{
    // if radio data is ready
    if (usbDataAvailable)
    {
        handleIncomingUSBMessage();
        usbDataAvailable = false;
    }
}

/*============================================================================
Name    :   handleIncomingUSBMessage
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from USB
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void handleIncomingUSBMessage()
{
    static RingBuffer usbBufferIn;

    RingBuffer_init(&usbBufferIn);
    uint8_t length = readUSB(usbBufferIn.buf);
    usbBufferIn.count = usbBufferIn.head = length;

    while (usbBufferIn.count > 0)
    {
        if (usbBufferIn.count >= 5)
        {
            if (RingBuffer_get(&usbBufferIn, 0) == '~')
            {
                uint8_t messageType = RingBuffer_get(&usbBufferIn, 1);
                uint8_t recipientId = RingBuffer_get(&usbBufferIn, 2);
                uint8_t payloadSize = RingBuffer_get(&usbBufferIn, 3);

                if ((payloadSize + 5) <= usbBufferIn.count)
                {
                    if (recipientId < myRobots.count && RingBuffer_get(&usbBufferIn, 4 + payloadSize) == '!')
                    {
                        Robot *r = RobotRingBuffer_findWithId(&myRobots, recipientId);
                        if (r)
                        {
                            r->payloadSize = payloadSize;
                            switch (messageType)
                            {
                            case TYPE_MOTORS_VELOCITY:
                                for (uint8_t i = 0; i < payloadSize; i++)
                                    r->dataBuffer[i] = RingBuffer_get(&usbBufferIn, 4 + i);
                                break;

                            default:
                                break;
                            }
                        }
                        RingBuffer_erase(&usbBufferIn, payloadSize + 5);
                    }
                    else
                        RingBuffer_pop(&usbBufferIn);
                }
                else
                    RingBuffer_flush(&usbBufferIn, 0);
            }
            else
                RingBuffer_pop(&usbBufferIn);
        }
        else
            RingBuffer_flush(&usbBufferIn, 0);
        ;
    }
}

/*============================================================================
Name    :   prepareNewRobot
------------------------------------------------------------------------------
Purpose :   prepares the id and pipe address for new robots to join
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
void assignNewRobot()
{
    static Robot newComingRobot = {0, DEFAULT_ADDRESS+2, 0, 0, {0}};

    if(myRobots.count <= NB_MAX_ROBOTS)
    {
        stopListening();
        openWritingPipe(DEFAULT_ADDRESS);
        Message msg;
        msg.header.type = TYPE_NEW_ROBOT;
        msg.header.id = RECEIVER_ID;
        msg.payload[0] = newComingRobot.id;
        memcpy_fast(&msg.payload[1], (uint8_t *)&newComingRobot.pipeAddress, sizeof(newComingRobot.pipeAddress));

        sendRadioMessage(&msg, sizeof(newComingRobot.id) + sizeof(newComingRobot.pipeAddress));

        if (DEBUG_ENABLED())
            debug_printf("New robot joined: id=%d | address=0x%x\n", newComingRobot.id, newComingRobot.pipeAddress);

        RobotRingBuffer_push(&myRobots, newComingRobot);

        newComingRobot.id++;
        newComingRobot.pipeAddress += 2;

        delayMicroseconds(500);//let some time to robot to change mode
    }
    else if(DEBUG_ENABLED())
        debug_printf("Can't add more robots!\n");  
}


/*============================================================================
Name    :   prepareNewRobot
------------------------------------------------------------------------------
Purpose :   ticks the watchdog timer of the given robot
Input   :   robotId: id of the robot
Output  :   
Return	:  
Notes   :
============================================================================*/
void tickRobotWatchdogTimer(uint8_t robotId)
{
    if (myRobots.count > 0 && robotId < myRobots.count)
    {
        Robot *r = RobotRingBuffer_get(&myRobots, robotId);
        if (r)
        {
          r->watchdogTimer++;
        }
    }
}

/*============================================================================
Name    :   resetRobotWatchdogTimer
------------------------------------------------------------------------------
Purpose :  resets the watchdog timer of the given robot
Input   :  id: id of the robot
Output  :   
Return	:  
Notes   :
============================================================================*/
void resetRobotWatchdogTimer(uint8_t robotId)
{
    if (myRobots.count > 0 && robotId < myRobots.count)
    {
        Robot *r = RobotRingBuffer_get(&myRobots, robotId);
        if (r)
        {
          r->watchdogTimer = 0;
        }
    }
}

/*============================================================================
Name    :   HAL_GPIO_EXTI_Callback
------------------------------------------------------------------------------
Purpose :   Handles the GPIO interrupt routines
Input   :   GPIO_Pin
            which pin has triggered the interrupt
Output  :   
Return	:  
Notes   :
============================================================================*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if ((GPIO_Pin & RADIO_IRQ_PIN) > 0)
        radioEvent = true;
}