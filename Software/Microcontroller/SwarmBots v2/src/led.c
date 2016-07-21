#include "led.h"

/* Timer handler declaration */
TIM_HandleTypeDef rgbTimHandle;
TIM_OC_InitTypeDef sRgbConfig;

bool isRGBInitialized = false;
/* RGB Led timer values */
uint32_t currentRed, currentGreen, currentBlue;

/*============================================================================
Name    :   initRGBLed
------------------------------------------------------------------------------
Purpose :   initializes the RGB Led, by configuring first all the pins, then 
	    the timer and configuring the PWM signals
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void initRGBLed()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    currentRed = currentGreen = currentBlue = 0;

    RGB_TIMER_CLK_ENABLE();
    RGB_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = RGB_GPIO_AF;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = RED_PIN | GREEN_PIN | BLUE_PIN;
    HAL_GPIO_Init(RGB_GPIO_PORT, &GPIO_InitStructure);

    /* Compute the prescaler value to have TIM1 counter clock equal to 4 000 000 Hz */
    uint32_t uhPrescalerValue = (uint32_t)(SystemCoreClock / RGB_CLK) - 1;

    rgbTimHandle.Instance = RGB_TIM;

    rgbTimHandle.Init.Prescaler = uhPrescalerValue;
    rgbTimHandle.Init.Period = RGB_PERIOD - 1;
    rgbTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    rgbTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    rgbTimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_PWM_Init(&rgbTimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sRgbConfig.OCMode = TIM_OCMODE_PWM1;
    sRgbConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sRgbConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sRgbConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sRgbConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sRgbConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    sRgbConfig.Pulse = 0;
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sRgbConfig, RED_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sRgbConfig, GREEN_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&rgbTimHandle, &sRgbConfig, BLUE_CHANNEL) != HAL_OK)
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
    isRGBInitialized = true;
    setRGBLed(0, 0, 0);
}

/*============================================================================
Name    :   setRGBLed
------------------------------------------------------------------------------
Purpose :   sets the red green and blue LED with a RGB value
Input   :   red: 0 to 255
	    green: 0 to 255
	    blue: 0 to 255
Output  :   none
Return	:
Notes   :
============================================================================*/
void setRGBLed(uint8_t red, uint8_t green, uint8_t blue)
{
    if (isRGBInitialized)
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
Name    :   setColor
------------------------------------------------------------------------------
Purpose :   sets the RGB led with the provided color from colors.h
Input   :   color: pointer to the color
Output  :   none
Return	:
Notes   :
============================================================================*/
void setColor(const uint8_t *color)
{
    if (color)
    {
        setRGBLed(color[0]/4, color[1]/4, color[2]/4);
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
    static uint16_t direction = 0, counter = 0;

    if (counter % 1000 == 0)
    {
        if (currentRed > RGB_PERIOD - 20)
            direction = 0;
        else if (currentRed <= RGB_PERIOD / 4)
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
        if (currentGreen >= RGB_PERIOD - 20)
            direction = 0;
        else if (currentGreen <= RGB_PERIOD / 4)
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
        if (currentBlue >= RGB_PERIOD - 20)
            direction = 0;
        else if (currentBlue <= RGB_PERIOD / 4)
            direction = 1;

        if (direction == 1)
            currentBlue++;
        else if (direction == 0)
            currentBlue--;

        setBlueLedRaw(currentBlue);
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
    currentRed = (currentRed <= 180) ? RGB_PERIOD : 180;
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
    currentGreen = (currentGreen <= 180) ? RGB_PERIOD : 180;
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
    currentBlue = (currentBlue <= 180) ? RGB_PERIOD : 180; //150;
    setBlueLedRaw(currentBlue);
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
void setBlueLed(uint8_t blue)
{
    currentBlue = (uint32_t)(RGB_PERIOD - RGB_PERIOD * blue / 255);
    setBlueLedRaw(currentBlue);
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
    if (redRaw <= RGB_PERIOD && isRGBInitialized)
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
    if (greenRaw <= RGB_PERIOD && isRGBInitialized)
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
void setBlueLedRaw(uint16_t blueRaw)
{
    if (blueRaw <= RGB_PERIOD && isRGBInitialized)
        __HAL_TIM_SetCompare(&rgbTimHandle, BLUE_CHANNEL, blueRaw);
}
