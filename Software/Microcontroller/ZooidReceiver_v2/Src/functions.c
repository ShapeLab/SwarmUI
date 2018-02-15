#include "functions.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t receiverId = 0;

volatile uint8_t zooidToPoll;
volatile bool canPollNextRobot;
volatile bool radioEvent;
volatile bool usbDataAvailable; //modified in stm32f0xx_it.c
bool usbReadyToSend;
bool receiverInitialized;
bool waitForUSBData;
bool managerListening;

RingBuffer usbBufferIn;
RingBuffer usbBufferOut;
RobotRingBuffer myZooids;
uint8_t activeZooids = 0;
uint8_t tmpBuffer[2048] = {0}; //not allocated

uint8_t updateFrequency = 60; //in Hz
uint16_t updatePeriod = 16;   //(1000/updateFrequency); //in ms
uint8_t numMaxZooid = 10;

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
  bool initOK = true;
  HAL_Init();
  /* Configure the system clock to have a system clock = 48 Mhz */
  SystemClock_Config();

  /*Configure peripheral I/O remapping */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
  __HAL_AFIO_REMAP_SPI1_ENABLE();

  zooidToPoll = numMaxZooid;
  canPollNextRobot = false;
  radioEvent = false;
  usbDataAvailable = false;
  usbReadyToSend = false;
  receiverInitialized = false;
  waitForUSBData = true;
  managerListening = false;

  initLeds();

  if (!initRadio(receiverId))
  {
    if (DEBUG_ENABLED())
      debug_printf("Radio not initialized correctly, ");
    Error_Handler();
    initOK = false;
  }

  if (initUSB() == false)
  {
    if (DEBUG_ENABLED())
      debug_printf("USB not initialized correctly, ");
    initOK = false;
  }
  setStatusLed(0);
  if (initOK)
    setGreenLed(10);
  else
    setRedLed(10);
}

/*============================================================================
Name    :   configureReceiver
------------------------------------------------------------------------------
Purpose :   configures the receiver with the parameters received from the 
            server.
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void configureReceiver()
{

  RobotRingBuffer_init(&myZooids);
  RingBuffer_init(&usbBufferIn);
  RingBuffer_init(&usbBufferOut);

  updatePeriod = (uint16_t)(1000 / updateFrequency);

  Robot newComingRobot;
  for (int i = 0; i < numMaxZooid; i++)
  {
    memset(&newComingRobot, 0, sizeof(newComingRobot));
    newComingRobot.id = i;
    newComingRobot.pipeAddress = DEFAULT_ADDRESS + 12 + newComingRobot.id * 3;
    newComingRobot.messageType = TYPE_UPDATE;
    RobotRingBuffer_push(&myZooids, newComingRobot);
  }

  Message m;
  m.header.id = receiverId;
  m.header.type = TYPE_RECEIVER_INFO;
  sendUSBMessage(&m, sizeof(m.header));

  if (DEBUG_ENABLED())
    debug_printf("Configured: id #%d - %dHz - %d Zooids\n", receiverId, updateFrequency, numMaxZooid);
  receiverInitialized = true;
  setStatusLed(200);
}

/*============================================================================
Name    :   checkRadio
------------------------------------------------------------------------------
Purpose :   manages the radio communication, i.e. polls all the known and 
            active robots and reacts to radio events
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void checkRadio()
{
  Message tmpMsg;
  Position tmpPosition;

  setBlueLed(activeZooids * 2);

  if (receiverInitialized && zooidToPoll < myZooids.count)
  {
    if (canPollNextRobot)
    {
      if (DEBUG_ENABLED())
        HAL_GPIO_WritePin(BLUE_GPIO_PORT, BLUE_PIN, HIGH);
      sendRobotUpdate(zooidToPoll);
      canPollNextRobot = false;
    }
  }

  if (radioEvent)
  {
    bool tx, fail, rx;
    whatHappened(&tx, &fail, &rx);
    clearInterruptFlag(rx, tx, fail);

    radioEvent = false;
    canPollNextRobot = true;
    zooidToPoll++;

    //if a message was received handle it
    if (rx)
    {
      if (DEBUG_ENABLED())
        HAL_GPIO_WritePin(BLUE_GPIO_PORT, BLUE_PIN, LOW);

      activeZooids++;

      handleIncomingRadioMessage();
      if (!isRxFifoEMpty())
        flush_rx();
    }
    //if the transmission didn't go through, discard it
    if (fail)
    {
      if (DEBUG_ENABLED())
        HAL_GPIO_WritePin(BLUE_GPIO_PORT, BLUE_PIN, LOW);

      if (getNbLostPacket() == 0x0F)
        resetNbLostPacket();

      flush_tx();

      if (DEBUG_ENABLED())
      {
        HAL_GPIO_WritePin(STATUS_GPIO_PORT, STATUS_PIN, HIGH);
        delayMicroseconds(2);
        HAL_GPIO_WritePin(STATUS_GPIO_PORT, STATUS_PIN, LOW);
      }
    }
  }
}

/*============================================================================
Name    :   handleIncomingRadioMessage
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from the radio and deals with different
            types of messages
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void handleIncomingRadioMessage()
{
  uint8_t payloadSize = getDynamicPayloadSize();

  // if the size is too big then the data isn't valid, just discard it
  if (payloadSize > PAYLOAD_MAX_SIZE)
    flush_rx();
  else if (payloadSize > 0)
  {
    Message msg;
    if (DEBUG_ENABLED())
      HAL_GPIO_WritePin(GREEN_GPIO_PORT, GREEN_PIN, HIGH);
    //saves the new message
    readRadio((uint8_t *)&msg, payloadSize);

    switch (msg.header.type)
    {
    //status data from the robot containing position, orientation & state,
    //just forward it through USB and poll the next robot
    case TYPE_STATUS:
      sendUSBMessage(&msg, payloadSize - sizeof(Header));

      break;
    default:
      flush_rx();
      break;
    }
    if (DEBUG_ENABLED())
      HAL_GPIO_WritePin(GREEN_GPIO_PORT, GREEN_PIN, LOW);
  }
}

/*============================================================================
Name    :   sendRobotUpdate
------------------------------------------------------------------------------
Purpose :   sends a message to the given robot with new updates and
            to ask for its status
Input   :   robotId : id of the robot to ask for position
Output  :   
Return	:  
Notes   :
============================================================================*/
void sendRobotUpdate(uint8_t robotId)
{
  int z;
  static unsigned char red = 0, g = 0, b = 0;
  if (myZooids.count > 0 && robotId < myZooids.count)
  {
    Robot *r = RobotRingBuffer_get(&myZooids, robotId);
    if (r)
    {
      selectRobot(r);
      Message msg;
      msg.header.type = r->messageType;
      msg.header.id = RECEIVER_ID;
      if (r->payloadSize <= PAYLOAD_MAX_SIZE)
      {
        memcpy_fast(msg.payload, r->dataBuffer, r->payloadSize);
        sendRadioMessage(&msg, r->payloadSize + sizeof(Header));
      }
    }
  }
}

/*============================================================================
Name    :   restartPolling
------------------------------------------------------------------------------
Purpose :  resets the variable to start a new round of polls
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
void restartPolling()
{
  if (receiverInitialized)
  {
    activeZooids = 0;
    zooidToPoll = 0;
    canPollNextRobot = true;
    usbReadyToSend = true;
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
  // if usb data is ready
  if (usbDataAvailable)
  {
    handleIncomingUSBMessage();
    usbDataAvailable = false;
    waitForUSBData = false;
  }
  processUSBData();
}

/*============================================================================
Name    :   handleIncomingUSBMessage
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from USB and stores it in a ringbuffer
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void handleIncomingUSBMessage()
{

  uint8_t length = readUSB(tmpBuffer);

  if (length)
  {
    for (uint16_t i = 0; i < length; i++)
    {
      RingBuffer_push(&usbBufferIn, tmpBuffer[i]);
    }
  }
}

/*============================================================================
Name    :   processUSBData
------------------------------------------------------------------------------
Purpose :   synchronizes and decodes USB data then store the data in the
            buffer of the concerned robot
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void processUSBData()
{
  uint8_t bytesToSend = 0;
  uint8_t tmpInBuffer = 0;
  uint8_t tmpOutBuffer[64] = {0};

  if (usbBufferIn.count >= 4 && !waitForUSBData)
  {
    if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 0) == true && tmpInBuffer == '~')
    {
      uint8_t messageType = 0, recipientId = 0, payloadSize = 0;

      if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 1) == true)
        messageType = tmpInBuffer;
      if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 2) == true)
        recipientId = tmpInBuffer;
      if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 3) == true)
        payloadSize = tmpInBuffer;

      if ((payloadSize + 4) < usbBufferIn.count)
      {
        if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 4 + payloadSize) == true && tmpInBuffer == '!')
        {
          if (recipientId == RECEIVER_ID)
          {
            if (messageType == TYPE_REBOOT)
            {
              Reboot();
            }
            else if (messageType == TYPE_RECEIVER_CONFIG)
            {
              ReceiverConfigMessage config;
              for (uint8_t i = 0; i < payloadSize; i++)
              {
                if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 4 + i) == true)
                  *((uint8_t *)&config + i) = tmpInBuffer;
              }
              updateFrequency = config.updateFrequency;
              numMaxZooid = config.numZooids;
              receiverId = config.receiverId;
              configureReceiver();
            }
            else if (messageType == TYPE_HANDSHAKE_REQUEST)
            {
              uint8_t tmpMessage[8] = {0};
              for (uint8_t i = 0; i < payloadSize; i++)
              {
                if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 4 + i) == true)
                  tmpMessage[i] = tmpInBuffer;
              }
              if (strcmp(tmpMessage, HANDSHAKE_REQUEST) == 0)
              {
                Message m;
                m.header.type = TYPE_HANDSHAKE_REPLY;
                m.header.id = MANAGER_ID;
                memcpy_fast(m.payload, HANDSHAKE_REPLY, sizeof(HANDSHAKE_REPLY));
                managerListening = true;
                usbReadyToSend = true;
                sendUSBMessage(&m, sizeof(HANDSHAKE_REPLY));
              }
            }
            else if (messageType == TYPE_HANDSHAKE_LEAVE)
            {
              uint8_t tmpMessage[8] = {0};
              for (uint8_t i = 0; i < payloadSize; i++)
              {
                if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 4 + i) == true)
                  tmpMessage[i] = tmpInBuffer;
              }
              if (strcmp(tmpMessage, HANDSHAKE_LEAVE) == 0)
              {
                managerListening = false;
                setStatusLed(0);
              }
            }
          }
          else if (recipientId < myZooids.count)
          {
            Robot *r = RobotRingBuffer_get(&myZooids, recipientId);
            if (r)
            {
              r->payloadSize = payloadSize;
              r->messageType = messageType;

              if (r->payloadSize <= PAYLOAD_MAX_SIZE)
              {
                switch (messageType)
                {
                case TYPE_MOTORS_VELOCITY:
                case TYPE_ROBOT_POSITION:
                  for (uint8_t i = 0; i < payloadSize; i++)
                  {
                    if (RingBuffer_get(&usbBufferIn, &tmpInBuffer, 4 + i) == true)
                      r->dataBuffer[i] = tmpInBuffer;
                  }
                  break;

                default:
                  break;
                }
              }
            }
          }
          RingBuffer_erase(&usbBufferIn, payloadSize + 5);
        }
        else
          RingBuffer_pop(&usbBufferIn);
      }
      else
        waitForUSBData = true;
    }
    else
      RingBuffer_pop(&usbBufferIn);
  }
  else
    waitForUSBData = true;

  if (usbBufferOut.count > 0)
  {
    bytesToSend = (usbBufferOut.count <= 64) ? usbBufferOut.count : 64;
    if (isUSBReadyToWrite() && usbReadyToSend)
    {
      for (uint8_t i = 0; i < bytesToSend; i++)
      {
        tmpOutBuffer[i] = RingBuffer_pop(&usbBufferOut);
      }
      writeUSB(bytesToSend, tmpOutBuffer);
    }
  }
  else
    usbReadyToSend = false;
}

/*============================================================================
Name    :   sendUSBMessage
------------------------------------------------------------------------------
Purpose :   
Input   :   
Output  :   
Return	:  
Notes   :
============================================================================*/
void sendUSBMessage(Message *msg, uint8_t payloadSize)
{
  if (msg != NULL && managerListening)
  {
    RingBuffer_push(&usbBufferOut, '~');
    RingBuffer_push(&usbBufferOut, msg->header.type);
    RingBuffer_push(&usbBufferOut, msg->header.id);
    RingBuffer_push(&usbBufferOut, payloadSize);

    for (int i = 0; i < payloadSize; i++)
      RingBuffer_push(&usbBufferOut, msg->payload[i]);

    RingBuffer_push(&usbBufferOut, '!');
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
  {
    radioEvent = true;
  }
}