#include "sensors.h"

/* ADC handle declaration */
ADC_HandleTypeDef AdcHandle;

/* Sensors data */
Quaternion quaternion;

bool isPhotoDiodesInitialized = false;
bool isSensorsInitialized = false;
/* Robot positioning variables */

RingBuffer positionBitBuffer[NB_PHOTODIODES];
Position photoDiodesPositions[NB_PHOTODIODES];
Position robotPosition;
float robotAngle;
bool firstBitEver[NB_PHOTODIODES];

bool readyToUpdate0 = false;
bool readyToUpdate1 = false;

volatile bool batteryLevelReady = false;
OrientationVec orientation;

uint8_t currentTouch = 0;

bool blinded = false;
bool tapped = false;
bool doubleTapped = false;
bool shaken = false;

bool caseId0 = false;
bool caseId1 = false;

uint32_t batteryLevel;
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

     orientation.x = orientation.y = orientation.z = 1;

    quaternion.w = 1.0f;
    quaternion.x = quaternion.y = quaternion.z = 0.0f;

    robotPosition.x = robotPosition.y = 500;
    robotAngle = 0.0f;

    initBatteryMonitor();

    initPhotoDiodes();

    if (I2CInit())
    {
        HAL_Delay(200);
        isSensorsInitialized = initImu() & initTouch();
    }

    if(isSensorsInitialized) 
        updateMotionInformation();


    initCaseReader();

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
Name    :   initBatteryMonitor
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
void initBatteryMonitor()
{
    /* ADC channel configuration structure declaration */
    ADC_ChannelConfTypeDef sConfig;

    /* ### - 1 - Initialize ADC peripheral #################################### */
    /*
   *  Instance                  = ADC1.
   *  ClockPrescaler            = PCLK divided by 4.
   *  LowPowerAutoWait          = Disabled
   *  LowPowerAutoPowerOff      = Disabled
   *  Resolution                = 12 bit (increased to 16 bit with oversampler)
   *  ScanConvMode              = ADC_SCAN_ENABLE 
   *  DataAlign                 = Right
   *  ContinuousConvMode        = Enabled
   *  DiscontinuousConvMode     = Enabled
   *  ExternalTrigConv          = ADC_SOFTWARE_START
   *  ExternalTrigConvEdge      = None (Software start)
   *  EOCSelection              = End Of Conversion event
   *  DMAContinuousRequests     = ENABLE
   */

    AdcHandle.Instance = ADC1;

    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    AdcHandle.Init.LowPowerAutoWait = DISABLE;
    AdcHandle.Init.LowPowerAutoPowerOff = DISABLE;
    AdcHandle.Init.Resolution = ADC_RESOLUTION_10B;
    AdcHandle.Init.ScanConvMode = ADC_SCAN_ENABLE;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.ContinuousConvMode = DISABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    AdcHandle.Init.DMAContinuousRequests = DISABLE;
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

    /* Initialize ADC peripheral according to the passed parameters */
    if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        Error_Handler();
    }

    /* ### - 2 - Start calibration ############################################ */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        Error_Handler();
    }

    /* ### - 3 - Channel configuration ######################################## */
    /* Select Channel 0 to be converted */
    sConfig.Channel = ADC_CHANNEL_VBAT;
    sConfig.Rank = ADC_RANK_NONE; //ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
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
    static uint32_t lastPositionTimestamp = 0;

    if (positionBitBuffer[0].count >= SIZEOF_PATTERN)
        readyToUpdate0 = calculatePhotodiodePosition(0);

    if (positionBitBuffer[1].count >= SIZEOF_PATTERN)
        readyToUpdate1 = calculatePhotodiodePosition(1);

    //if all photodiodes have decoded their patterns
    if (readyToUpdate0 == true && readyToUpdate1 == true)
    {
        lastPositionTimestamp = HAL_GetTick();
        robotPosition.x = (photoDiodesPositions[1].x + photoDiodesPositions[0].x) / 2;
        robotPosition.y = (photoDiodesPositions[1].y + photoDiodesPositions[0].y) / 2;

        robotAngle = atan2f((float)(photoDiodesPositions[1].y - photoDiodesPositions[0].y), (float)(photoDiodesPositions[1].x - photoDiodesPositions[0].x)) * RAD_TO_DEG_CST + 90.0f;

        if(robotAngle > 180.0f)
            robotAngle -= 360.0f;

        readyToUpdate0 = false;
        readyToUpdate1 = false;

        blinded = false;
        return true;
    }
    // no new position for a while then stop
    else if(HAL_GetTick()-lastPositionTimestamp > 100)
        blinded = true;

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
Position *getRobotPosition()
{
    return &robotPosition;
}

/*============================================================================
Name    :   getRobotAngle
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
float *getRobotAngle()
{
    return &robotAngle;
}

/*============================================================================
Name    :   startReadBatteryLevel
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
void startReadBatteryLevel()
{
    /* ### - 4 - Start conversion in DMA mode ################################# */
    if (isSensorsInitialized)
    {
        if (HAL_ADC_Start_DMA(&AdcHandle, &batteryLevel, 1) != HAL_OK)
        {
            Error_Handler();
        }
    }
}

/*============================================================================
Name    :   getBatteryLevel
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
uint8_t getBatteryLevel()
{
    if(batteryLevelReady && DEBUG_ENABLED())
    {
        debug_printf("%d\n", batteryLevel);
        batteryLevelReady =  false;
    }
    batteryLevelReady =  false;
    return (batteryLevel < 530) ? 0 : 100;
}

/*============================================================================
Name    :   isCharging
------------------------------------------------------------------------------
Purpose :   returns the current orienation of the robot
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
bool isCharging()
{
    return (HAL_GPIO_ReadPin(CHARGING_STATUS_GPIO_PORT, CHARGING_STATUS_PIN) == LOW) ? true : false;
}

/*============================================================================
Name    :   updateMotionInformation
------------------------------------------------------------------------------
Purpose :   as the IMU has triggered an interrupt, fetch the source and
            process the data accordingly
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
void updateMotionInformation()
{
    updateTap();
    updateOrientation();
}

/*============================================================================
Name    :   getInteractionState
------------------------------------------------------------------------------
Purpose :   compile all the interactions into one bitmap:
            | 7 | 6 | 5 |    4    |     3     |   2  |   1   |   0   |
            --------------------------------------------------------
            | - | - | - | Shaken | DoubleTap | Tap | Blind | Touch |
Input   :
Output  :
Return	:   a bitmap containing the state for each interaction
Notes   :
============================================================================*/
uint8_t getInteractionState()
{
    uint8_t state = 0;
     
    if(currentTouch > 0) state |= 1;
    if(blinded) state |= 2;
    if(tapped)  state |= 4;
    if(doubleTapped)  state |= 8;
    if(shaken)  state |= 16;
    if(caseId0) state |= 32;
    if(caseId1) state |= 64;    
    return state;
}

/*============================================================================
Name    :   isBlinded
------------------------------------------------------------------------------
Purpose :   indicates if the robot is blinded
Input   :
Output  :
Return	:   boolean: true if blinded, false otherwise
Notes   :
============================================================================*/
bool isBlinded()
{
   return blinded;
}

/*============================================================================
Name    :   isTouched
------------------------------------------------------------------------------
Purpose :   indicates if the robot is touched or not
Input   :
Output  :
Return	:   boolean: true for touched, false for untouched
Notes   :
============================================================================*/
bool isTouched()
{
    return (currentTouch>0);
}

/*============================================================================
Name    :   updateTouch
------------------------------------------------------------------------------
Purpose :   fetches the new touch status
Input   :
Output  :
Return	:   return  true if the sensor has not pulled down the interrupt
Notes   :
============================================================================*/
bool updateTouch()
{
    currentTouch = readTouchKeyStatus();
    if(HAL_GPIO_ReadPin(TOUCH_CHANGE_GPIO_PORT, TOUCH_CHANGE_PIN) == HIGH)
      return true;
    else
      return false;
}

/*============================================================================
Name    :   updateOrientation
------------------------------------------------------------------------------
Purpose :   acquires the last IMU measurements and updates the quaternion
Input   :   interrupt source from
Output  :   none
Return	:
Notes   :
============================================================================*/
void updateOrientation()
{
//    MotionData m;
//    uint32_t currentTimestep = 0;
//    uint32_t previousTimestamp = 0;
//
//    uint32_t currentTimestamp = HAL_GetTick();
//
//    currentTimestep = currentTimestamp - previousTimestamp;
//    getMotionT(&m);
//    MadgwickAHRSupdateIMU(&quaternion, m.gx, m.gy, m.gz, m.ax, m.ay, m.az, currentTimestep);
//    previousTimestamp = currentTimestamp;
    
    uint8_t interruptSrc = getD6DSource();
    if(interruptSrc != 0xff)
    {   
        if(interruptSrc & LSM6DS3_INT_FLAG_XH) orientation.x = 1;
        if(interruptSrc & LSM6DS3_INT_FLAG_XL) orientation.x = -1;
        if(interruptSrc & LSM6DS3_INT_FLAG_YH) orientation.y = 1;
        if(interruptSrc & LSM6DS3_INT_FLAG_YL) orientation.y = -1;
        if(interruptSrc & LSM6DS3_INT_FLAG_ZH) orientation.z = 1;
        if(interruptSrc & LSM6DS3_INT_FLAG_ZL) orientation.z = -1;
    }
}

/*============================================================================
Name    :   updateTap
------------------------------------------------------------------------------
Purpose :   acquires the last IMU measurements and updates the quaternion
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void updateTap()
{
    uint8_t interruptSrc = getTapSource();
    if(interruptSrc != 0xff)
    {
        if(interruptSrc & LSM6DS3_INT_FLAG_SINGLE_TAP)
        {
            tapped = true;
        }
        if(interruptSrc & LSM6DS3_INT_FLAG_DOUBLE_TAP)
        {
            doubleTapped = true;
        }

    }
}

void initCaseReader() 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    CASE_SEL_0_GPIO_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = CASE_SEL_0_PIN;
    HAL_GPIO_Init(CASE_SEL_0_GPIO_PORT, &GPIO_InitStructure);
    
    CASE_SEL_1_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = CASE_SEL_1_PIN;
    HAL_GPIO_Init(CASE_SEL_1_GPIO_PORT, &GPIO_InitStructure);
}

uint8_t readCaseId()
{
    uint8_t caseId = 0;

    if(HAL_GPIO_ReadPin(CASE_SEL_0_GPIO_PORT, CASE_SEL_0_PIN) == HIGH) {
      caseId += 1;
      caseId0 = true;
    }
    else 
      caseId0 = false;

    if(HAL_GPIO_ReadPin(CASE_SEL_1_GPIO_PORT, CASE_SEL_1_PIN) == HIGH){
      caseId += 2;
      caseId1 = true;
    }
    else
      caseId1 = false;
   
      return caseId;
}