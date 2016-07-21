#include "functions.h"
#include "colors.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Timer Output Compare Configuration Structure declaration */

/* Peripherals initialized states */

/* Interrupt flags */
volatile bool touchChanged = false;
volatile bool radioEvent = false;
bool positionSent = true;
uint8_t currentTouch = 0;
bool isAddressValid = false;
volatile CHARGING_STATE_t chargingStatus = DISCONNECTED;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*============================================================================
Name    :   upateRobot
------------------------------------------------------------------------------
Purpose :   updates the position and orientation of the robot and prepare the
            message to be sent
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void initRobot()
{
    HAL_Init();
    SystemClock_Config();

    initRGBLed();

    initMotors();

    if (!initSensors())
        Error_Handler();

    if (!initRadio())
        Error_Handler();

    initPhotoDiodes();
    chargingStatus = (HAL_GPIO_ReadPin(CHARGING_STATUS_GPIO_PORT, CHARGING_STATUS_PIN) == LOW) ? CHARGING : CHARGED;
}

/*============================================================================
Name    :   upateRobot
------------------------------------------------------------------------------
Purpose :   updates the position and orientation of the robot and prepare the
            message to be sent
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void updateRobot()
{
  
    if (updateRobotPosition())
        prepareMessageToSend(getRobotPosition(), getRobotOrientation(), &currentTouch);
}

/*============================================================================
Name    :   sleep
------------------------------------------------------------------------------
Purpose :   Sleeping procedure (TODO)
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void sleep()
{
    SleepMode_Measure();
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
    // if radio data is ready
    if (radioEvent)
    {
        bool tx, fail, rx;
        whatHappened(&tx, &fail, &rx);

        if (rx)
        {
            handleIncomingRadioMessage();
            if (isRxFifoEMpty())
            {
                radioEvent = false;
                clearInterruptFlag(true, false, false);
            }
            resetCommunicationWatchdog();
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
    if (remainingCommunicationWatchdog() == 0)
    {
        isAddressValid = false;
        sendAddressRequest();
    }
}

/*============================================================================
Name    :   handleIncomingMessage
------------------------------------------------------------------------------
Purpose :   acquires new incoming data from the radio
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void handleIncomingRadioMessage()
{
    Position tmpPosition = {0,0};
    uint64_t tmpPipeAddress = 0;
    uint8_t payloadSize = getDynamicPayloadSize();
    if (payloadSize > PAYLOAD_MAX_SIZE)
        flush_rx();
    else
    {
        Message msg;
        memset(&msg, 0, sizeof(msg));
        readRadio((uint8_t *)&msg, payloadSize);

        if (msg.header.id == RECEIVER_ID)
        {
            switch (msg.header.type)
            {
            case TYPE_UPDATE:
            case TYPE_MOTORS_VELOCITY:
                setMotor1((int8_t)msg.payload[0]);
                setMotor2((int8_t)msg.payload[1]);
                setRGBLed(msg.payload[2] / 8, msg.payload[3] / 8, msg.payload[4] / 8);

                prepareMessageToSend(getRobotPosition(), getRobotOrientation(), &currentTouch);
                break;

            case TYPE_NEW_ROBOT:
                if(!isAddressValid)
                {
                    setRobotId(msg.payload[0]);
                    memcpy((uint8_t *)&tmpPipeAddress, &msg.payload[1], sizeof(tmpPipeAddress));
                    setRobotPipeAddress(tmpPipeAddress);

                    openCommunication();
                    startListening();

                    if (DEBUG_ENABLED())
                        debug_printf("Info received, id=%d | address=0x%x\n", getRobotId(), getRobotPipeAddress());
                    isAddressValid = true;
                }
                break;

            case TYPE_REBOOT_ROBOT:
                Reboot();
                break;
            default:
                break;
            }
        }
    }
}

/*============================================================================
Name    :   prepareMessageToSend
------------------------------------------------------------------------------
Purpose :   prepares the new position message and stacks it as
            a new ack packet
Input   :   *position : pointer to the robot's current position
            *orientation : pointer to the robot's current orientation
            which pin has triggered the interrupt
Output  :
Return	:
Notes   :
============================================================================*/
void prepareMessageToSend(Position *position, float *orientation, uint8_t *touch)
{
    int16_t tmpOrientation = 0;
    Message msg;
    msg.header.id = getRobotId();
    msg.header.type = TYPE_STATUS;

    if (isAddressValid)
    {
        if (position && orientation && touch)
        {
            tmpOrientation = (int16_t)(*orientation * 100.0f);


            memcpy_fast(msg.payload, (uint8_t *)position, sizeof(*position));
            memcpy_fast(msg.payload + sizeof(*position), (uint8_t *)&tmpOrientation, sizeof(tmpOrientation));
            memcpy_fast(msg.payload + sizeof(*position) + sizeof(tmpOrientation), touch, sizeof(*touch));

            //            memcpy_fast(msg.payload+sizeof(*position)+sizeof(*tmpOrientation)+sizeof(*touch), &photoDiodesPositions[0], 2*sizeof(Position));

            writeAckPayload(0, (uint8_t *)&msg, sizeof(Header) + sizeof(*position) + sizeof(tmpOrientation) + sizeof(*touch)); // + 2*sizeof(Position));
        }
    }
}

/*============================================================================
Name    :   checkTouch
------------------------------------------------------------------------------
Purpose :   acquires touch information when a new event happened and then
            stores it
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void checkTouch()
{
    // if a new touch happened
    if (touchChanged)
    {
        currentTouch = readQTKeyStatus();
        if (HAL_GPIO_ReadPin(TOUCH_CHANGE_GPIO_PORT, TOUCH_CHANGE_PIN) == HIGH)
        {
            touchChanged = false;
        }
    }
}

/*============================================================================
Name    :   switchToChargingMode
------------------------------------------------------------------------------
Purpose :   put the robot into sleep mode while charging
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void switchToChargingMode()
{
    setGreenLed(0);
    setBlueLed(0);
    powerDown();
    setMotor1(0);
    setMotor2(0);

    while (chargingStatus == CHARGING)
    {
        glowRedLed();
    }

    powerUp();
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
    //Touch change IRQ
    if (GPIO_Pin & TOUCH_CHANGE_PIN)
        touchChanged = true;

    if (GPIO_Pin & PHOTODIODE_1_PIN)
        readPhotoDiode(0);
    //
    if (GPIO_Pin & PHOTODIODE_2_PIN)
        readPhotoDiode(1);

    //charger IRQ
    if ((GPIO_Pin & CHARGING_STATUS_PIN) > 0)
    {
        chargingStatus = (HAL_GPIO_ReadPin(CHARGING_STATUS_GPIO_PORT, CHARGING_STATUS_PIN) == LOW) ? CHARGING : CHARGED;
        if (chargingStatus == CHARGING)
            switchToChargingMode();
    }

    //nRF IRQ
    if ((GPIO_Pin & RADIO_IRQ_PIN) > 0)
        radioEvent = true;

    //IMU IRQ
    //    if(GPIO_Pin && IMU_INT_PIN)
}