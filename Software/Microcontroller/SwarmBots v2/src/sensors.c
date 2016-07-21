#include "sensors.h"

/* Sensors data */
Quaternion quaternion;

bool isPhotoDiodesInitialized = false;
bool isSensorsInitialized = false;
/* Robot positioning variables */

RingBuffer positionBitBuffer[NB_PHOTODIODES];
Position photoDiodesPositions[NB_PHOTODIODES];
Position robotPosition;
float robotOrientation;
bool firstBitEver[NB_PHOTODIODES];

bool readyToUpdate0 = false;
bool readyToUpdate1 = false;

/*============================================================================
Name    :   initSensors
------------------------------------------------------------------------------
Purpose :   initializes the sensors, with the I2C bus first, then the QT1070
	    touch sensor and finally the IMU
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
bool initSensors()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure the INT pins */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = TOUCH_CHANGE_PIN;
    HAL_GPIO_Init(TOUCH_CHANGE_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pin = CHARGING_STATUS_PIN;
    HAL_GPIO_Init(CHARGING_STATUS_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pin = IMU_INT_PIN;
    HAL_GPIO_Init(IMU_INT_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(TOUCH_CHANGE_IRQn, TOUCH_CHANGE_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TOUCH_CHANGE_IRQn);

    HAL_NVIC_SetPriority(CHARGING_STATUS_IRQn, CHARGING_STATUS_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(CHARGING_STATUS_IRQn);

    HAL_NVIC_SetPriority(IMU_INT_IRQn, IMU_INT_IRQ_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(IMU_INT_IRQn);

    quaternion.w = 1.0f;
    quaternion.x = quaternion.y = quaternion.z = 0.0f;

    robotPosition.x = robotPosition.y = 0;
    robotOrientation = 0.0f;

    if (I2CInit())
    {
        HAL_Delay(100);
        isSensorsInitialized = initIMU() & initQtInterface();
    }
    return isSensorsInitialized;
}

/*============================================================================
Name    :   initPhotoDiodes
------------------------------------------------------------------------------
Purpose :   initializes the two photodiodes, by configuring first the pins
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void initPhotoDiodes()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    PHOTODIODE_1_GPIO_CLK_ENABLE();
    PHOTODIODE_2_GPIO_CLK_ENABLE();

    /* Configure the GPIO pin */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = PHOTODIODE_1_PIN;
    HAL_GPIO_Init(PHOTODIODE_1_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(PHOTODIODE_1_IRQn, PHOTODIODE_1_IRQ_PRIORITY, 0);

    GPIO_InitStructure.Pin = PHOTODIODE_2_PIN;
    HAL_GPIO_Init(PHOTODIODE_2_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(PHOTODIODE_2_IRQn, PHOTODIODE_2_IRQ_PRIORITY, 0);

    for (uint8_t i = 0; i < NB_PHOTODIODES; i++)
    {
        firstBitEver[i] = true;
        RingBuffer_init(&positionBitBuffer[i]);
    }
    isPhotoDiodesInitialized = true;

    HAL_NVIC_EnableIRQ(PHOTODIODE_1_IRQn);
    HAL_NVIC_EnableIRQ(PHOTODIODE_2_IRQn);
}

/*============================================================================
Name    :   updateOrientation
------------------------------------------------------------------------------
Purpose :   acquires the last IMU measurements and updates the quaternion
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void updateOrientation()
{
    MotionData m;
    uint32_t currentTimestep = 0;
    uint32_t previousTimestamp = 0;

    uint32_t currentTimestamp = HAL_GetTick();

    currentTimestep = currentTimestamp - previousTimestamp;
    getMotionT(&m);
    MadgwickAHRSupdateIMU(&quaternion, m.gx, m.gy, m.gz, m.ax, m.ay, m.az, currentTimestep);
    previousTimestamp = currentTimestamp;
}

/*============================================================================
Name    :   readPhotoDiodes
------------------------------------------------------------------------------
Purpose :   reads the two photodiodes 
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void readPhotoDiode(uint8_t photodiodeId)
{
    static uint64_t previousTime[NB_PHOTODIODES] = {0};
    uint8_t value = 0;
    uint64_t currentTime = 0;
    uint64_t period = 0;
    uint32_t nbBits = 0;

    if (isPhotoDiodesInitialized)
    {
        if (firstBitEver[photodiodeId] == false)
        {
            currentTime = getMicroSeconds();
            period = currentTime - previousTime[photodiodeId];
            nbBits = period / BIT_DURATION;
            previousTime[photodiodeId] = currentTime;

            // if remainder is greater than half of BIT_DURATION, increase n by 1
            if (period % BIT_DURATION > BIT_DURATION / 2)
                nbBits++;

            // get the negative as it represents the previous state
            if (photodiodeId == 0)
                value = !HAL_GPIO_ReadPin(PHOTODIODE_1_GPIO_PORT, PHOTODIODE_1_PIN);
            else if (photodiodeId == 1)
                value = !HAL_GPIO_ReadPin(PHOTODIODE_2_GPIO_PORT, PHOTODIODE_2_PIN);

            //save bits, even if the pattern is all black, all white or white bits before the end pattern
            //otherwise it means it didn't see the pattern properly so discard the bits
            if (nbBits <= 2 * SIZEOF_END)
            {
                for (uint8_t i = 0; i < nbBits; i++)
                    RingBuffer_push(&positionBitBuffer[photodiodeId], value);
            }
        }
        else
        {
            previousTime[photodiodeId] = getMicroSeconds();
            firstBitEver[photodiodeId] = false;
        }
    }
}

/*============================================================================
Name    :   calculatePhotodiodePosition
------------------------------------------------------------------------------
Purpose :   Calculate the row and column for one photodiode
Input   :   photodiodeId : id of the photodiode 
Output  :   
Return	:   bool : if new position has been calculated
Notes   :
============================================================================*/
bool calculatePhotodiodePosition(uint8_t photodiodeId)
{
    /* Read PRIMASK register, check interrupt status before you disable them */
    /* Returns 0 if they are enabled, or non-zero if disabled */
    uint32_t prim = __get_PRIMASK();
    bool result = false;
    bool endFound = true;

    if (RingBuffer_get(&positionBitBuffer[photodiodeId], 0) == BLACK)
    {
        for (uint8_t i = 0; i < SIZEOF_END; i++)
        {
            if (RingBuffer_get(&positionBitBuffer[photodiodeId], i + SIZEOF_START + SIZEOF_COLUMN + SIZEOF_ROW) != WHITE)
            {
                endFound = false;
                break;
            }
        }

        if (endFound)
        {
            __disable_irq();
            //decode gray code for x
            uint16_t grayTmp = RingBuffer_get(&positionBitBuffer[photodiodeId], SIZEOF_START);
            photoDiodesPositions[photodiodeId].x = (uint16_t)((uint16_t)grayTmp << (SIZEOF_COLUMN - 1));
            for (uint8_t i = 1 + SIZEOF_START; i <= SIZEOF_COLUMN; i++)
            {
                grayTmp = grayTmp ^ RingBuffer_get(&positionBitBuffer[photodiodeId], i);
                photoDiodesPositions[photodiodeId].x += (uint16_t)((uint16_t)grayTmp << (SIZEOF_COLUMN - i));
            }

            //decode gray code for y
            grayTmp = RingBuffer_get(&positionBitBuffer[photodiodeId], SIZEOF_START + SIZEOF_COLUMN);
            photoDiodesPositions[photodiodeId].y = (uint16_t)((uint16_t)grayTmp << (SIZEOF_ROW - 1));
            for (uint8_t i = 1 + SIZEOF_START + SIZEOF_COLUMN; i <= SIZEOF_COLUMN + SIZEOF_ROW; i++)
            {
                grayTmp = grayTmp ^ RingBuffer_get(&positionBitBuffer[photodiodeId], i);
                photoDiodesPositions[photodiodeId].y += (uint16_t)((uint16_t)grayTmp << (SIZEOF_COLUMN + SIZEOF_ROW - i));
            }

            RingBuffer_erase(&positionBitBuffer[photodiodeId], SIZEOF_PATTERN);

            if (!prim)
                __enable_irq();

            return true;
        }
        else
            RingBuffer_pop(&positionBitBuffer[photodiodeId]);
    }
    else
        RingBuffer_pop(&positionBitBuffer[photodiodeId]);
    return false;
}

/*============================================================================
Name    :   updateRobotPosition
------------------------------------------------------------------------------
Purpose :   Updates the positioning information from the photodiodes' readings
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
bool updateRobotPosition()
{
    if (positionBitBuffer[0].count >= SIZEOF_PATTERN)
        readyToUpdate0 = calculatePhotodiodePosition(0);

    if (positionBitBuffer[1].count >= SIZEOF_PATTERN)
        readyToUpdate1 = calculatePhotodiodePosition(1);

    //if all photodiodes have decoded their patterns
    if (readyToUpdate0 == true && readyToUpdate1 == true)
    {
        robotPosition.x = (photoDiodesPositions[1].x + photoDiodesPositions[0].x) / 2;
        robotPosition.y = (photoDiodesPositions[1].y + photoDiodesPositions[0].y) / 2;
  
        robotOrientation = atan2f((float)(photoDiodesPositions[1].y - photoDiodesPositions[0].y),
                                        (float)(photoDiodesPositions[1].x - photoDiodesPositions[0].x)) * RAD_TO_DEG_CST + 90.0f;
//        angle = (angle > 0.0f) ? 180.0f - angle : -180.0f - angle;

        readyToUpdate0 = false;
        readyToUpdate1 = false;
        return true;
    }
    return false;
}

/*============================================================================
Name    :   getRobotPosition
------------------------------------------------------------------------------
Purpose :   returns the current position of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
Position* getRobotPosition()
{
  return &robotPosition;
}

/*============================================================================
Name    :   getRobotOrientation
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
float* getRobotOrientation()
{
  return &robotOrientation;
}
