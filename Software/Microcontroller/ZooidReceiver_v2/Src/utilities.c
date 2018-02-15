#include "utilities.h"
#include "functions.h"
#include "nRF24L01.h"

/* Private typedef -----------------------------------------------------------*/
enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

/* Timer handler declaration */
TIM_HandleTypeDef microTimHandle;

/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;

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

bool SPIInit()
{
    /*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    SpiHandle.Instance = SPIx;

    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 8- 9MHz
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
        return false;
    else
    {
	__HAL_SPI_ENABLE(&SpiHandle);
        return true;
    }
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED2 on: Transfer in transmission/reception process is complete */
  wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  wTransferState = TRANSFER_ERROR;
}

uint8_t SPIWriteRegisters(uint8_t registerAddress, uint8_t length, uint8_t *data)
{
    uint8_t bufferTx[PAYLOAD_MAX_SIZE + 1] = {0}; //+1 to store the register
    uint8_t bufferRx[PAYLOAD_MAX_SIZE] = {0xff};

    bufferTx[0] = registerAddress;

    memcpy_fast((uint8_t *)&bufferTx[1], (uint8_t *)data, length);

    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, length + 1);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);

    return bufferRx[0];
}

uint8_t SPIWriteRegister(uint8_t registerAddress, uint8_t data)
{
    return SPIWriteRegisters(registerAddress, 1, (uint8_t *)&data);
}

uint8_t SPIWriteCommand(uint8_t cmd)
{
    uint8_t bufferTx[2] = {cmd, 0xff};
    uint8_t bufferRx[2] = {0xff};

    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, LOW);
    HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t *)bufferTx, (uint8_t *)bufferRx, 1);
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY);
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
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY);
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
    while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY);
    HAL_GPIO_WritePin(SPIx_CSN_GPIO_PORT, SPIx_CSN_PIN, HIGH);
    *data = bufferRx[1];

    return bufferRx[0];
}

void SystemClock_Config(void)
{
    SystemClockHSE_Config();

    initMicroTimer();

    if (DEBUG_ENABLED())
        debug_printf("UC running at %dMHz\n", HAL_RCC_GetSysClockFreq() / 1000000);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 16000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClockHSE_Config(void)
{
    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    oscinitstruct.HSEState = RCC_HSE_ON;
    oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
    oscinitstruct.PLL.PLLState = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | 
			       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
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
        for (uint64_t i = 0; i < 400000; i++);
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
        for (uint64_t i = 0; i < 400000; i++);
    }
}
#endif