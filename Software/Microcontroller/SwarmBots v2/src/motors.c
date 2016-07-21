#include "motors.h"

TIM_HandleTypeDef motorsTimHandle;
TIM_OC_InitTypeDef sMotorsConfig;
bool isMotorsInitialized = false;

/*============================================================================
Name    :   initMotors
------------------------------------------------------------------------------
Purpose :   initializes the motors, by configuring first all the pins, then 
	    the timer and configuring the PWM signals
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void initMotors()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    MOTORS_CLK_ENABLE();
    MOTOR1_GPIO_CLK_ENABLE();
    MOTOR2_GPIO_CLK_ENABLE();
    MOTOR3_GPIO_CLK_ENABLE();
    MOTOR4_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = MOTORS_GPIO_AF;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

    GPIO_InitStructure.Pin = MOTOR1_PIN | MOTOR2_PIN | MOTOR3_PIN | MOTOR4_PIN;
    HAL_GPIO_Init(MOTOR2_GPIO_PORT, &GPIO_InitStructure);

    /* Compute the prescaler value to have TIM2 counter clock equal to 4 000 000 Hz */
    uint32_t uhPrescalerValue = (uint32_t)(SystemCoreClock / MOTORS_CLK) - 1;

    motorsTimHandle.Instance = MOTORS_TIM;

    motorsTimHandle.Init.Prescaler = uhPrescalerValue;
    motorsTimHandle.Init.Period = MOTORS_PERIOD - 1;
    motorsTimHandle.Init.ClockDivision = 0;
    motorsTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    motorsTimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_PWM_Init(&motorsTimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sMotorsConfig.OCMode = TIM_OCMODE_PWM1;
    sMotorsConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sMotorsConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sMotorsConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sMotorsConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sMotorsConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    /* Set the pulse value for channel 1 */
    sMotorsConfig.Pulse = 0;
    if (HAL_TIM_PWM_ConfigChannel(&motorsTimHandle, &sMotorsConfig, MOTOR1_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&motorsTimHandle, &sMotorsConfig, MOTOR2_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&motorsTimHandle, &sMotorsConfig, MOTOR3_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&motorsTimHandle, &sMotorsConfig, MOTOR4_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&motorsTimHandle, MOTOR1_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&motorsTimHandle, MOTOR2_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&motorsTimHandle, MOTOR3_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&motorsTimHandle, MOTOR4_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }
    setMotor1(0);
    setMotor2(0);
    isMotorsInitialized = true;
}

/*============================================================================
Name    :   setMotor1
------------------------------------------------------------------------------
Purpose :   sets motor1 direction and speed

	    speed : indicates the speed of rotation 
		    (0 to MOTORS_PERIOD)
Output  :   none
Return	:
Notes   :
============================================================================*/
void setMotor1(int8_t speed)
{
    int8_t trueSpeed = -speed;
    uint8_t motor1 = 0, motor2 = 0;
    if (trueSpeed >= 0)
    {
        motor1 = (trueSpeed <= MOTORS_PERIOD) ? trueSpeed : MOTORS_PERIOD;
        motor2 = 0;
    }
    else
    {
        motor1 = 0;
        motor2 = (-trueSpeed <= MOTORS_PERIOD) ? -trueSpeed : MOTORS_PERIOD;
    }

    __HAL_TIM_SetCompare(&motorsTimHandle, MOTOR1_CHANNEL, motor1);
    __HAL_TIM_SetCompare(&motorsTimHandle, MOTOR2_CHANNEL, motor2);
}

/*============================================================================
Name    :   setMotor2
------------------------------------------------------------------------------
Purpose :   sets motor2 direction and speed

	    speed : indicates the speed of rotation 
		    (0 to MOTORS_PERIOD)
Output  :   none
Return	:
Notes   :
============================================================================*/
void setMotor2(int8_t speed)
{
    int8_t trueSpeed = -speed;
    uint8_t motor3 = 0, motor4 = 0;

    if (trueSpeed >= 0)
    {
        motor3 = (trueSpeed <= MOTORS_PERIOD) ? trueSpeed : MOTORS_PERIOD;
        motor4 = 0;
    }
    else
    {
        motor3 = 0;
        motor4 = (-trueSpeed <= MOTORS_PERIOD) ? -trueSpeed : MOTORS_PERIOD;
    }
    __HAL_TIM_SetCompare(&motorsTimHandle, MOTOR3_CHANNEL, motor3);
    __HAL_TIM_SetCompare(&motorsTimHandle, MOTOR4_CHANNEL, motor4);
}