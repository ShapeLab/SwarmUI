#include "utilities.h"
#include "functions.h"
#include "nRF24L01.h"

#define BUFFER_SIZE (PAYLOAD_SIZE + 1)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;
/* Timer handler declaration */
TIM_HandleTypeDef microTimHandle;

extern uint64_t microTick;

void initMicroTimer()
{
    TIM_ClockConfigTypeDef microTimConfig;

    uint32_t uhPrescalerValue = (uint32_t)(SystemCoreClock / MICRO_CLK) - 1;

    microTimHandle.Instance = MICRO_TIM;
    microTimHandle.Init.Prescaler = uhPrescalerValue;
    microTimHandle.Init.Period = MICRO_PERIOD;
    microTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    microTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    microTimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&microTimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    microTimConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    microTimConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING;
    microTimConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    microTimConfig.ClockFilter = 0;

    if (HAL_TIM_ConfigClockSource(&microTimHandle, &microTimConfig) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    if (HAL_TIM_Base_Start_IT(&microTimHandle) != HAL_OK)
    {
        Error_Handler();
    }
}

void delayMicroseconds(uint32_t micros)
{
    uint64_t tickStart = getMicroSeconds();
    while ((getMicroSeconds() - tickStart) < micros)
    {
    }
}

uint64_t getMicroSeconds()
{
    return microTick + (microTimHandle.Instance->CNT);
}

bool I2CInit()
{
    I2cHandle.Instance = I2Cx;
    I2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.Timing = I2Cx_TIMING;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress1 = 0;
    I2cHandle.Init.OwnAddress2 = 0;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
        return false;
    return true;
}

bool SPIInit()
{
    /*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    SpiHandle.Instance = SPIx;

    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLED;
    SpiHandle.Init.CRCLength = SPI_CRC_LENGTH_16BIT;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
        return false;
    else
        return true;
}

bool SPIFlushRxFifo()
{
    if (HAL_SPI_FlushRxFifo(&SpiHandle) == HAL_OK)
        return true;
    else
        return false;
}

uint8_t SPIWriteRegisters(uint8_t registerAddress, uint8_t length, uint8_t *data)
{
    uint8_t bufferTx[PAYLOAD_MAX_SIZE + 1] = {0}; //+1 to store the register
    uint8_t bufferRx[PAYLOAD_MAX_SIZE] = {0xff};

    bufferTx[0] = registerAddress;

    memcpy_fast((uint8_t *)&bufferTx[1], (uint8_t *)data, length);

    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, length + 1);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
        ;
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);

    return bufferRx[0];
}

uint8_t SPIWriteRegister(uint8_t registerAddress, uint8_t data)
{
    return SPIWriteRegisters(registerAddress, 1, (uint8_t *)&data);
}

uint8_t SPIWriteCommand(uint8_t cmd, uint8_t *value)
{
    uint8_t bufferTx[2] = {cmd, 0xff};
    uint8_t bufferRx[2] = {0xff};

    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    if (value)
    {
        HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, 2);
        *value = bufferRx[1];
    }
    else
        HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, 1);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
        ;
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);

    return bufferRx[0];
}

uint8_t SPIReadBytes(uint8_t registerAddress, uint8_t length, uint8_t *data)
{
    uint8_t bufferTx[PAYLOAD_MAX_SIZE] = {0xff};
    uint8_t bufferRx[PAYLOAD_MAX_SIZE] = {0};
    bufferTx[0] = registerAddress;

    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, length + 1);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
        ;
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);
    memcpy_fast(data, (uint8_t *)&bufferRx[1], length);

    return bufferRx[0];
}

uint8_t SPIReadByte(uint8_t registerAddress, uint8_t *data)
{
    uint8_t bufferTx[2] = {registerAddress, 0xff};
    uint8_t bufferRx[2] = {0xff};
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, 2);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
        ;
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);
    *data = bufferRx[1];

    return bufferRx[0];
}

bool I2CWriteBytes(uint8_t sensorAddress, uint8_t registerAddress, uint8_t length, uint8_t *data)
{
    if (HAL_I2C_Mem_Write(&I2cHandle, sensorAddress, registerAddress, 1, data, length, 10) == HAL_OK)
        return true;
    else
        return false;
}

bool I2CWriteByte(uint8_t sensorAddress, uint8_t registerAddress, uint8_t data)
{
    return I2CWriteBytes(sensorAddress, registerAddress, 1, &data);
}

bool I2CWriteBit(uint8_t sensorAddress, uint8_t registerAddress, uint8_t bitNumber, bool data)
{
    uint8_t regValue = 0;
    if (I2CReadByte(sensorAddress, registerAddress, &regValue) == HAL_OK)
    {
        regValue = (data != 0) ? (regValue | (1 << bitNumber)) : (regValue & ~(1 << bitNumber));
        if (I2CWriteByte(sensorAddress, registerAddress, regValue) == HAL_OK)
            return true;
    }
    return false;
}

bool I2CReadBytes(uint8_t sensorAddress, uint8_t registerAddress, uint8_t length, uint8_t *data)
{
    if (HAL_I2C_Mem_Read(&I2cHandle, sensorAddress, registerAddress, 1, data, length, 1000) == HAL_OK)
        return true;
    else
        return false;
}

bool I2CReadByte(uint8_t sensorAddress, uint8_t registerAddress, uint8_t *data)
{
    return I2CReadBytes(sensorAddress, registerAddress, 1, data);
}

bool I2CReadBit(uint8_t sensorAddress, uint8_t registerAddress, uint8_t bitNumber, bool *data)
{
    uint8_t tmp = 0;
    bool b = I2CReadBytes(sensorAddress, registerAddress, 1, &tmp);
    if (b)
    {
        *data = ((tmp & (1 << bitNumber)) > 0) ? true : false;
        return true;
    }
    else
        return false;
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    if (DEBUG_ENABLED())
        debug_printf("HAL_I2C_MemTxCpltCallback\n");
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    if (DEBUG_ENABLED())
        debug_printf("HAL_I2C_MemRxCpltCallback\n");
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
    if (DEBUG_ENABLED())
        debug_printf("HAL_I2C_ErrorCallback\n");
}

void SystemClock_Config(void)
{
    SystemClockHSE_Config();
    if (DEBUG_ENABLED())
        debug_printf("UC running at %dMHz\n", HAL_RCC_GetSysClockFreq() / 1000000);

    initMicroTimer();
}

/**
* @brief  Switch the PLL source from HSI48 to HSI, and select the PLL as SYSCLK
*         source.
*         The system Clock is configured as follows :
*            System Clock source            = PLL (HSI)
*            SYSCLK(Hz)                     = 48000000
*            HCLK(Hz)                       = 48000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 1
*            HSE Frequency(Hz)              = 16000000
*            PREDIV                         = 2
*            PLLMUL                         = 6
*            Flash Latency(WS)              = 1
* @param  None
* @retval None
*/
static void SystemClockHSE_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* -1- Select HSI as system clock source to allow modification of the PLL configuration */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }

    /* -2- Enable HSE Oscillator, select it as PLL source and finally activate the PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    /* PLL source is the HSE frequency multipled by 6 */
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* -3- Select the PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* -4- Optional: Disable HSI48 Oscillator (if the HSI48 is no more needed by the application)*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

/**  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    int i = 0;
    /* User may add here some code to deal with this error */
    setRGBLed(0, 0, 0);
    while (i < 10)
    {
        toggleRedLed();
        for (uint64_t i = 0; i < 400000; i++)
            ;
        i++;
    }
    if (DEBUG_ENABLED())
        debug_printf("Something went wrong, rebooting...\n");
    Reboot();
}

void Reboot()
{
    NVIC_SystemReset();
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if ((*pBuffer1) != *pBuffer2)
        {
            return BufferLength;
        }
        pBuffer1++;
        pBuffer2++;
    }

    return 0;
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
        toggleRedLed();
        for (uint64_t i = 0; i < 400000; i++)
            ;
    }
}
#endif