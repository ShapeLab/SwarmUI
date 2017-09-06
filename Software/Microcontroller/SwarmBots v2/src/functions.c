#include "functions.h"
#include "colors.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Timer Output Compare Configuration Structure declaration */

uint8_t robotId = 0;
/* Peripherals initialized states
/* Interrupt flags */
volatile bool touchEvent = false;
volatile bool radioEvent = false;
volatile bool imuEvent = false;
bool isAddressValid = true;
volatile bool charging = false;
Target currentGoal = {500, 500, 0, 1};
Motor motorValues = {0, 0, 1.0f, 22, MAX_SPEED, 60};
bool isPositionControl = true;


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

    if (DEBUG_ENABLED())
        debug_printf("Starting...\n");

    for(int i=0;i<3000;i+=500)
    {
        HAL_Delay(250);
        setColor(yellow);
        HAL_Delay(250);
        setColor(black);
    }

    if (!initSensors())
        Error_Handler();

    if (!initRadio(robotId))
        Error_Handler();
    prepareStatusToSend(getRobotPosition(), getRobotAngle(), getInteractionState(), getBatteryLevel());

    while(calibrate(&motorValues) == false);

    charging = isCharging();

    setRGBLed(0,5,0);
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
    bool reached = false;
    bool aligned = false;

//    uint8_t caseId =  readCaseId();

    if (updateRobotPosition())
    { 
        prepareStatusToSend(getRobotPosition(), getRobotAngle(), getInteractionState(), getBatteryLevel());
        if (isPositionControl)
        {
            //angleControl(0,&motorValues,&aligned, false, 5.0f);
            positionControl(currentGoal.x, currentGoal.y, currentGoal.angle, &motorValues, &reached, false, currentGoal.finalGoal);
            //    calibrate(&motorValues);

            minimumc(&motorValues.motor1, motorValues.minVelocity);
            minimumc(&motorValues.motor2, motorValues.minVelocity);
            maximumc(&motorValues.motor1, motorValues.maxVelocity);
            maximumc(&motorValues.motor2, motorValues.maxVelocity);

            setMotor1(motorValues.motor1);
            setMotor2(motorValues.motor2 * motorValues.motorGain);
            //TODO change motorGain to be an int and divide by 1000 or so
        }
    }

    if(isBlinded() || isTouched())
    {
        setMotor1(0);
        setMotor2(0);
    }
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
        clearInterruptFlag(rx, tx, fail);

        radioEvent = false;
        if (rx)
            handleIncomingRadioMessage();
        if (fail)
        {
            if (getNbLostPacket() == 0x0F)
                resetNbLostPacket(); 
            flush_tx();
        }
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
    Position tmpPosition = {0, 0};
    uint64_t tmpPipeAddress = 0;
    uint8_t payloadSize = getDynamicPayloadSize();
    PositionControlMessage* positionMessage;
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
                prepareStatusToSend(getRobotPosition(), getRobotAngle(), getInteractionState(), getBatteryLevel());
                break;
            case TYPE_MOTORS_VELOCITY:
                isPositionControl = false;
                setMotor1((int8_t)msg.payload[0]);
                setMotor2((int8_t)msg.payload[1]);
                setRGBLed(msg.payload[2] / 8, msg.payload[3] / 8, msg.payload[4] / 8);
                prepareStatusToSend(getRobotPosition(), getRobotAngle(), getInteractionState(), getBatteryLevel());
                break;
            case TYPE_ROBOT_POSITION:
                isPositionControl = true;
                positionMessage = (PositionControlMessage*)msg.payload;

                currentGoal.x = positionMessage->positionX;
                currentGoal.y = positionMessage->positionY;
                setRGBLed(positionMessage->colorRed/8, positionMessage->colorGreen/8, positionMessage->colorBlue/8); 
                currentGoal.angle = ((float)positionMessage->orientation)/100.0f;

                motorValues.preferredVelocity = positionMessage->preferredSpeed;
                if(motorValues.preferredVelocity > MAX_SPEED)
                  motorValues.preferredVelocity = MAX_SPEED;
                if(motorValues.preferredVelocity < -MAX_SPEED)
                  motorValues.preferredVelocity = -MAX_SPEED;

//                currentGoal.finalGoal = (bool)positionMessage->isFinalGoal; 
                prepareStatusToSend(getRobotPosition(), getRobotAngle(), getInteractionState(), getBatteryLevel());

                break;
            case TYPE_NEW_ROBOT_INFORMATION:
                if (!isAddressValid)
                {
                    memcpy((uint8_t *)&tmpPipeAddress, &msg.payload[1], sizeof(tmpPipeAddress));
                    setRobotId(msg.payload[0]);
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
Name    :   prepareStatusToSend
------------------------------------------------------------------------------
Purpose :   prepares the new position message and stacks it as
            a new ack packet
Input   :   *position : pointer to the robot's current position
            *orientation : pointer to the robot's current orientation
            touch : current touch value (so small no need for pointer here)
            battery : current battery level (so small no need for pointer here)
Output  :
Return	:
Notes   :
============================================================================*/
void prepareStatusToSend(Position *position, float *orientation, uint8_t touch, uint8_t battery)
{
    static int16_t tmpOrientation = 0;
    Message msg;
    StatusMessage* status = (StatusMessage*)msg.payload;
    msg.header.id = getRobotId();
    msg.header.type = TYPE_STATUS;
    
    if (isAddressValid)
    {
        if (position && orientation)
        {
            status->positionX = position->x;
            status->positionY = position->y;
            status->batteryLevel = battery;
            status->touch = touch;
            status->orientation = (int16_t)(*orientation * 100.0f);

            prepareAnswer(&msg, sizeof(Header) + sizeof(StatusMessage));
        }
    }
}

/*============================================================================
Name    :   sendAddressRequest
------------------------------------------------------------------------------
Purpose :   `s a message to the given robot to ask for its position
Input   :   robotId : id of the robot to ask for position
Output  :   
Return	:  
Notes   :
============================================================================*/
void sendAddressRequest()
{
    if (DEBUG_ENABLED())
        debug_printf("Sending address request...\n");

    setGreenLed(5);
    stopListening();
    Message msg;

    setRobotPipeAddress(DEFAULT_ADDRESS);
    openCommunication();

    msg.header.type = TYPE_NEW_ROBOT_REQUEST;
    msg.header.id = DEFAULT_ID;

    sendRadioMessage(&msg, sizeof(Header));
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
  static uint32_t lastPoll = 0;
    // if a new touch happened
//    if (touchEvent)
//        touchEvent = updateTouch();

  if(HAL_GetTick() - lastPoll > 20){
    updateTouch(); // FOR SOME REASON THE FLAG DOESNT RELIABLY GO DOWN, SO POLLING...
    lastPoll = HAL_GetTick();
    
//    if(isTouched()) setRedLed(10);
//    else restoreColors();
  }
}

/*============================================================================
Name    :   checkCharger
------------------------------------------------------------------------------
Purpose :   looks if the robot is charging or not 
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void checkCharger()
{
    if (charging)
    {
      if(DEBUG_ENABLED())
        debug_printf("Charging...\n");
      switchToChargingMode();  
    }
}

/*============================================================================
Name    :   checkCharger
------------------------------------------------------------------------------
Purpose :   looks if the robot is charging or not 
Input   :
Output  :
Return	:
Notes   :
============================================================================*/
void checkIMU()
{
    if (imuEvent)
    {
//        updateMotionInformation();
        imuEvent = false;
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
    saveColors();
    setGreenLed(0);
    setBlueLed(0);
    powerDown();
    setMotor1(0);
    setMotor2(0);
    while (charging)
        glowRedLed();
    powerUp();
    restoreColors();
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
        touchEvent = true;

    if (GPIO_Pin & PHOTODIODE_1_PIN)
        readPhotoDiode(0);
    //
    if (GPIO_Pin & PHOTODIODE_2_PIN)
        readPhotoDiode(1);

    //charger IRQ
    if (GPIO_Pin & CHARGING_STATUS_PIN)
        charging = isCharging();

    //nRF IRQ
    if (GPIO_Pin & RADIO_IRQ_PIN)
        radioEvent = true;

    //IMU IRQ
    if(GPIO_Pin & IMU_INT_PIN)
        imuEvent = true;

}
