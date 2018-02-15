/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/stm32f1xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    29-April-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"   
#include "stm32f1xx_it.h"    
#include "functions.h"

/** @addtogroup Validation_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declared in "usbd_cdc_interface.c" file */
extern PCD_HandleTypeDef hpcd;
extern UART_HandleTypeDef UartHandle;

/* TIM handler declared in "usbd_cdc_interface.c" file */
extern TIM_HandleTypeDef TimHandle;
extern TIM_HandleTypeDef microTimHandle;

extern SPI_HandleTypeDef SpiHandle;

extern bool usbDataAvailable;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
volatile uint64_t microTick = 0;

extern uint8_t robotToPoll;
extern uint16_t updatePeriod;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    int i = 0;
    /* User may add here some code to deal with this error */
    setRGBLed(0, 0, 0);
    while (i < 20)
    {
        toggleRedLed();
        for (uint64_t i = 0; i < 400000; i++)
            ;
        i++;
    }
    if (DEBUG_ENABLED())
        debug_printf("Something went wrong, rebooting...\n");
    NVIC_SystemReset();
}


/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    HardFault_Handler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
   HAL_IncTick();

   if(HAL_GetTick() % updatePeriod == 0)
      restartPolling();
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f1xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles USB Handler.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  */
void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
  usbDataAvailable = true;
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}

/**
  * @brief  This function handles DMA Rx interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SpiHandle.hdmarx);
}

/**
  * @brief  This function handles DMA Tx interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SpiHandle.hdmatx);
}

void EXTI9_5_IRQHandler(uint16_t GPIO_Pin)
{
    /* EXTI line interrupt detected */
    HAL_GPIO_EXTI_IRQHandler(RADIO_IRQ_PIN);
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void MICRO_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&microTimHandle);
    microTick += MICRO_PERIOD;
}
/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
