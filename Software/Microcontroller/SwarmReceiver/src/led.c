#include "led.h"

/* Timer handler declaration */
TIM_HandleTypeDef rgbTimHandle;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;
/* RGB Led timer values */
uint32_t currentRed, currentGreen, currentBlue, statusLed;
/* Peripherals initialized states */
bool isLedsInitialized = false;

/*============================================================================
Name    :   initLeds
------------------------------------------------------------------------------
Purpose :   initializes the RGB Led and the Status Led, by configuring first 
	    all the pins, then the timer and configuring the PWM signals
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void initLeds()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    currentRed = currentGreen = currentBlue = statusLed = 0;

    /* Enable the GPIO_LED Clock */
    LED_REDBLUE_GPIO_CLK_ENABLE();
    LED_GREEN_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM3;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = RED_PIN | BLUE_PIN;
    HAL_GPIO_Init(RED_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GREEN_PIN | STATUS_PIN;
    HAL_GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStructure);

    /* Compute the prescaler value to have TIM1 counter clock equal to 4 000 000 Hz */
    uint32_t uhPrescalerValue = (uint32_t)(SystemCoreClock / RGB_CLK) - 1;

    rgbTimHandle.Instance = RGB_TIM;

    rgbTimHandle.Init.Prescaler = uhPrescalerValue;
    rgbTimHandle.Init.Period = RGB_PERIOD - 1;
    rgbTimHandle.Init.ClockDivision = 0;
    rgbTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    rgbTimHandle.Init.RepetitionCounter = 0;

    RGB_CLK_ENABLE();

    if (HAL_TIM_PWM_Init(&rgbTimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    sConfig.Pulse = 0;
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sConfig, RED_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sConfig, GREEN_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sConfig, BLUE_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sConfig, STATUS_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&rgbTimHandle, RED_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&rgbTimHandle, GREEN_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&rgbTimHandle, BLUE_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&rgbTimHandle, STATUS_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    isLedsInitialized = true;
    setRGBLed(0, 0, 0);
    setStatusLed(0);
}

/*============================================================================
Name    :   setRGBLed
------------------------------------------------------------------------------
Purpose :   sets the redm green and blue LED with a RGB value
Input   :   red: 0 to 255
	    green: 0 to 255
	    blue: 0 to 255
Output  :   none
Return	:
Notes   :
============================================================================*/
void setRGBLed(uint8_t red, uint8_t green, uint8_t blue)
{
    if (isLedsInitialized)
    {
        currentRed = (uint32_t)(RGB_PERIOD - RGB_PERIOD * red / 255);
        currentGreen = (uint32_t)(RGB_PERIOD - RGB_PERIOD * green / 255);
        currentBlue = (uint32_t)(RGB_PERIOD - RGB_PERIOD * blue / 255);

        __HAL_TIM_SetCompare(&rgbTimHandle, RED_CHANNEL, currentRed);
        __HAL_TIM_SetCompare(&rgbTimHandle, GREEN_CHANNEL, currentGreen);
        __HAL_TIM_SetCompare(&rgbTimHandle, BLUE_CHANNEL, currentBlue);
    }
}

/*============================================================================
Name    :   glowRedLed
------------------------------------------------------------------------------
Purpose :   make the red LED glow
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void glowRedLed()
{
    static uint8_t direction = 0, counter = 0;

    if (counter % 5 == 0)
    {
        if (currentRed >= 395)
            direction = 0;
        else if (currentRed <= 240)
            direction = 1;

        if (direction == 1)
            currentRed++;
        else if (direction == 0)
            currentRed--;

        setRedLedRaw(currentRed);
    }
    counter++;
}

/*============================================================================
Name    :   glowGreenLed
------------------------------------------------------------------------------
Purpose :   make the green LED glow
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void glowGreenLed()
{
    static uint8_t direction = 0, counter = 0;

    if (counter % 5 == 0)
    {
        if (currentGreen >= 395)
            direction = 0;
        else if (currentGreen <= 240)
            direction = 1;

        if (direction == 1)
            currentGreen++;
        else if (direction == 0)
            currentGreen--;

        setGreenLedRaw(currentGreen);
    }
    counter++;
}

/*============================================================================
Name    :   glowBlueLed
------------------------------------------------------------------------------
Purpose :   make the blue LED glow
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void glowBlueLed()
{
    static uint8_t direction = 0, counter = 0;

    if (counter % 5 == 0)
    {
        if (currentBlue >= 395)
            direction = 0;
        else if (currentBlue <= 240)
            direction = 1;

        if (direction == 1)
            currentBlue++;
        else if (direction == 0)
            currentBlue--;

        setBLueLedRaw(currentBlue);
    }
    counter++;
}

/*============================================================================
Name    :   glowStatusLed
------------------------------------------------------------------------------
Purpose :   make the status LED glow
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void glowStatusLed()
{
    static uint8_t direction = 0, counter = 0;

    if (counter % 5 == 0)
    {
        if (statusLed >= 395)
            direction = 0;
        else if (statusLed <= 240)
            direction = 1;

        if (direction == 1)
            statusLed++;
        else if (direction == 0)
            statusLed--;

        setStatusLedRaw(statusLed);
    }
    counter++;
}

/*============================================================================
Name    :   toggleRedLed
------------------------------------------------------------------------------
Purpose :   toggles the red LED
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void toggleRedLed()
{
    currentRed = (currentRed <= 3*RGB_PERIOD/4) ? RGB_PERIOD : 3*RGB_PERIOD/4;
    setRedLedRaw(currentRed);
}

/*============================================================================
Name    :   toggleGreenLed
------------------------------------------------------------------------------
Purpose :   toggles the green LED
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void toggleGreenLed()
{
    currentGreen = (currentGreen <= 3*RGB_PERIOD/4) ? RGB_PERIOD : 3*RGB_PERIOD/4;
    setGreenLedRaw(currentGreen);
}

/*============================================================================
Name    :   toggleBlueLed
------------------------------------------------------------------------------
Purpose :   toggles the blue LED
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void toggleBlueLed()
{
    currentBlue = (currentBlue <= 3*RGB_PERIOD/4) ? RGB_PERIOD : 3*RGB_PERIOD/4;
    setBLueLedRaw(currentBlue);
}

/*============================================================================
Name    :   toggleStatusLed
------------------------------------------------------------------------------
Purpose :   toggles the status LED
Input   :   none
Output  :   none
Return	:
Notes   :
============================================================================*/
void toggleStatusLed()
{
    statusLed = (statusLed <= 3*RGB_PERIOD/4) ? RGB_PERIOD : 3*RGB_PERIOD/4;
    setStatusLedRaw(statusLed);
}

/*============================================================================
Name    :   setRedLed
------------------------------------------------------------------------------
Purpose :   sets red LED
Input   :   red: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void setRedLed(uint8_t red)
{
    currentRed = (uint32_t)(RGB_PERIOD - RGB_PERIOD * red / 255);
    setRedLedRaw(currentRed);
}

/*============================================================================
Name    :   setGreenLed
------------------------------------------------------------------------------
Purpose :   sets greeen LED
Input   :   green: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void setGreenLed(uint8_t green)
{
    currentGreen = (uint32_t)(RGB_PERIOD - RGB_PERIOD * green / 255);
    setGreenLedRaw(currentGreen);
}

/*============================================================================
Name    :   setBlueLed
------------------------------------------------------------------------------
Purpose :   sets blue LED
Input   :   blue: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void setBLueLed(uint8_t blue)
{
    currentBlue = (uint32_t)(RGB_PERIOD - RGB_PERIOD * blue / 255);
    setBLueLedRaw(currentBlue);
}

/*============================================================================
Name    :   setStatusLed
------------------------------------------------------------------------------
Purpose :   sets status LED
Input   :   status: 0 to 255 value
Output  :   none
Return	:
Notes   :
============================================================================*/
void setStatusLed(uint8_t status)
{
    statusLed = (uint32_t)(RGB_PERIOD - RGB_PERIOD * status / 255);
    setStatusLedRaw(statusLed);
}

/*============================================================================
Name    :   setRedLed
------------------------------------------------------------------------------
Purpose :   sets red LED
Input   :   red: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void blinkRedLed()
{
    static uint32_t lastTime = 0;
    if((HAL_GetTick()-lastTime) >= BLINK_PERIOD)
    {
      lastTime = HAL_GetTick();
      toggleRedLed();
    }
}

/*============================================================================
Name    :   setGreenLed
------------------------------------------------------------------------------
Purpose :   sets greeen LED
Input   :   green: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void blinkGreenLed()
{
    static uint32_t lastTime = 0;
    if((HAL_GetTick()-lastTime) >= BLINK_PERIOD)
    {
      lastTime = HAL_GetTick();
      toggleGreenLed();
    }
}

/*============================================================================
Name    :   setBlueLed
------------------------------------------------------------------------------
Purpose :   sets blue LED
Input   :   blue: 0 to 255 value to be able to do standard RGB Colors
Output  :   none
Return	:
Notes   :
============================================================================*/
void blinkBlueLed()
{
    static uint32_t lastTime = 0;
    if((HAL_GetTick()-lastTime) >= BLINK_PERIOD)
    {
      lastTime = HAL_GetTick();
      toggleBlueLed();
    }
}

/*============================================================================
Name    :   blinkStatusLed
------------------------------------------------------------------------------
Purpose :   blinks the status LED at a precise frequency
Input   :   status: 0 to 255 value
Output  :   none
Return	:
Notes   :
============================================================================*/
void blinkStatusLed()
{
    static uint32_t lastTime = 0;
    if((HAL_GetTick()-lastTime) >= BLINK_PERIOD)
    {
      lastTime = HAL_GetTick();
      toggleStatusLed();
    }
}

/*============================================================================
Name    :   setRedLedRaw
------------------------------------------------------------------------------
Purpose :   sets red LED's timer value
Input   :   red : 0 to RGB_PERIOD value
Output  :   none
Return	:
Notes   :
============================================================================*/
void setRedLedRaw(uint16_t redRaw)
{
    if (redRaw <= RGB_PERIOD && isLedsInitialized)
        __HAL_TIM_SetCompare(&rgbTimHandle, RED_CHANNEL, redRaw);
}

/*============================================================================
Name    :   setGreenLedRaw
------------------------------------------------------------------------------
Purpose :   sets green LED's timer value
Input   :   green : 0 to RGB_PERIOD value
Output  :   none
Return	:
Notes   :
============================================================================*/
void setGreenLedRaw(uint16_t greenRaw)
{
    if (greenRaw <= RGB_PERIOD && isLedsInitialized)
        __HAL_TIM_SetCompare(&rgbTimHandle, GREEN_CHANNEL, greenRaw);
}

/*============================================================================
Name    :   setBlueLedRaw
------------------------------------------------------------------------------
Purpose :   sets blue LED's timer value
Input   :   blue : 0 to RGB_PERIOD value
Output  :   none
Return	:
Notes   :
============================================================================*/
void setBLueLedRaw(uint16_t blueRaw)
{
    if (blueRaw <= RGB_PERIOD && isLedsInitialized)
        __HAL_TIM_SetCompare(&rgbTimHandle, BLUE_CHANNEL, blueRaw);
}

/*============================================================================
Name    :   setStatusLedRaw
------------------------------------------------------------------------------
Purpose :   sets status LED's timer value
Input   :   blue : 0 to RGB_PERIOD value
Output  :   none
Return	:
Notes   :
============================================================================*/
void setStatusLedRaw(uint16_t statusRaw)
{
    if (statusRaw <= RGB_PERIOD && isLedsInitialized)
        __HAL_TIM_SetCompare(&rgbTimHandle, STATUS_CHANNEL, statusRaw);
}
