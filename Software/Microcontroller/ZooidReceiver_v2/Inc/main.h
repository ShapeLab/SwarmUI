
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include <__cross_studio_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "stm32f1xx_hal_pcd.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"


/* Exported macro ------------------------------------------------------------*/
#define BIT(x) (1 << (x))
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#ifdef DEBUG
  #define DEBUG_ENABLED() (TRUE)
#else
  #define DEBUG_ENABLED() (FALSE)
#endif
/*
  CONSTANTS DECLARATIONS
*/

#define USE_HSE

#define FALSE                                   0
#define TRUE                                    1
#define LOW                                     GPIO_PIN_RESET
#define HIGH                                    GPIO_PIN_SET

#define ON                                      TRUE
#define OFF                                     FALSE
#define false                                   FALSE
#define true                                    TRUE

///////////////////////////////////
//    COMMUNICATION CONSTANTS    //
///////////////////////////////////

#define RADIO_CHANNEL                           22//42//88  //channel at which you want to communicate through
#define PAYLOAD_MAX_SIZE                        32
#define ADDRESS_INCREMENT                       2
#define DEFAULT_ADDRESS                         0xF0F0F0F0D000
#define DEFAULT_ID                              0xFF
#define RECEIVER_ID                             250
#define MANAGER_ID                              251

#define HANDSHAKE_REQUEST                       "Are you?"
#define HANDSHAKE_REPLY                         "You are?"
#define HANDSHAKE_LEAVE                         "Bye!"

///////// MESSAGE TYPES ///////////
//Operating messages
#define TYPE_UPDATE                             0x03
#define TYPE_STATUS                             0x04
#define TYPE_MOTORS_VELOCITY                    0x05
#define TYPE_ROBOT_POSITION                     0x06
#define TYPE_RECEIVER_INFO                      0x10
#define TYPE_RECEIVER_CONFIG                    0x11
#define TYPE_HANDSHAKE_REQUEST                  0x12
#define TYPE_HANDSHAKE_REPLY                    0x13
#define TYPE_HANDSHAKE_LEAVE                    0x14

//Configuration Messages
#define TYPE_NEW_ROBOT_REQUEST                  0xF0
#define TYPE_NEW_ROBOT_INFORMATION              0xF1

#define TYPE_REBOOT                             0xFF
  
/*
  DELAY TIMER DECLARATIONS
*/
#define MICRO_TIM                               TIM4
#define MICRO_FREQ                              100
#define MICRO_CLK                               1000000
#define MICRO_PERIOD                            (uint32_t)(MICRO_CLK/MICRO_FREQ)  /* Period Value  */
#define MICRO_TIM_CLK_ENABLE()                  __HAL_RCC_TIM4_CLK_ENABLE();
#define MICRO_IRQn                              TIM4_IRQn
#define MICRO_IRQHandler                        TIM4_IRQHandler
#define MICRO_IRQ_PRIORITY                      0 //3->0
/*
  LED DECLARATIONS
*/
#define LED_REDBLUE_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_GREEN_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()

#define RED_PIN					GPIO_PIN_1
#define RED_GPIO_PORT                           GPIOB
#define GREEN_PIN				GPIO_PIN_7
#define GREEN_GPIO_PORT                         GPIOA
#define BLUE_PIN				GPIO_PIN_0
#define BLUE_GPIO_PORT                          GPIOB

#define STATUS_PIN                              GPIO_PIN_6
#define STATUS_GPIO_PORT                        GPIOA

#define BLINKING_FREQ                           5
#define BLINKING_PERIOD                         1000/BLINKING_FREQ
#define RGB_TIM					TIM3
#define RGB_FREQ				10000
#define RGB_CLK					1000000
#define RGB_PERIOD				(uint32_t)(RGB_CLK/RGB_FREQ)  /* Period Value  */
#define GREEN_CHANNEL				TIM_CHANNEL_2
#define RED_CHANNEL				TIM_CHANNEL_4
#define BLUE_CHANNEL				TIM_CHANNEL_3
#define STATUS_CHANNEL				TIM_CHANNEL_1
#define RGB_CLK_ENABLE()                        __HAL_RCC_TIM3_CLK_ENABLE()

/*
  RADIO DECLARATIONS
*/
/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                            GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT                      GPIOB
#define SPIx_SCK_AF                             GPIO_AF0_SPI1
#define SPIx_SCK_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
  
#define SPIx_MISO_PIN                           GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT                     GPIOB
#define SPIx_MISO_AF                            GPIO_AF0_SPI1
#define SPIx_MISO_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_MOSI_PIN                           GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT                     GPIOB
#define SPIx_MOSI_AF                            GPIO_AF0_SPI1
#define SPIx_MOSI_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
    
#define SPIx_CSN_PIN                            GPIO_PIN_15
#define SPIx_CSN_GPIO_PORT                      GPIOA
#define SPIx_CSN_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_CE_PIN                             GPIO_PIN_6
#define SPIx_CE_GPIO_PORT                       GPIOB
#define SPIx_CE_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()

#define RADIO_IRQ_PIN                           GPIO_PIN_7
#define RADIO_IRQ_GPIO_PORT                     GPIOB
#define RADIO_IRQn                              EXTI9_5_IRQn
#define RADIO_IRQ_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define RADIO_IRQ_PRIORITY                      1 //3->0

#define SPIx                                    SPI1
#define SPIx_CLK_ENABLE()                       __HAL_RCC_SPI1_CLK_ENABLE()

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_CHANNEL                      DMA1_Channel3
#define SPIx_RX_DMA_CHANNEL                      DMA1_Channel2

/* Definition for SPIx's NVIC */  
#define SPIx_DMA_TX_IRQn                        DMA1_Channel3_IRQn
#define SPIx_DMA_RX_IRQn                        DMA1_Channel2_IRQn

#define SPIx_DMA_TX_IRQHandler                  DMA1_Channel3_IRQHandler
#define SPIx_DMA_RX_IRQHandler                  DMA1_Channel2_IRQHandler

#define DMAx_CLK_ENABLE()                       __HAL_RCC_DMA1_CLK_ENABLE()

#define SPIx_FORCE_RESET()                      __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()                    __HAL_RCC_SPI1_RELEASE_RESET()


/* Uncomment the line below to select your USB clock source */
//#define USE_USB_CLKSOURCE_CRSHSI48   1
#define USE_USB_CLKSOURCE_PLL                   1

#if !defined (USE_USB_CLKSOURCE_PLL) && !defined (USE_USB_CLKSOURCE_CRSHSI48)
 #error "Missing USB clock definition"
#endif

/* Exported types ------------------------------------------------------------*/
typedef uint8_t MotorDirection;
typedef uint8_t bool;

typedef struct {
    uint8_t receiverId;
    uint8_t numZooids;
    uint8_t updateFrequency;
    uint8_t empty;
} ReceiverConfigMessage;

typedef struct
{
  uint16_t x;
  uint16_t y;
} Position;

typedef struct
{
  uint8_t type;
  uint8_t id;
} Header;

typedef struct 
{
  Header header;
  uint8_t payload[PAYLOAD_MAX_SIZE-sizeof(Header)];
} Message;

typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t T;
} ImuRawData;

typedef struct
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float T;
} MotionData;

typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;
} AccelRawData;

typedef struct
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
} GyroRawData;

typedef struct
{
    float w;
    float x;
    float y;
    float z;
} Quaternion;

typedef struct
{
    uint8_t id;
    uint64_t pipeAddress;
    uint8_t payloadSize;
    uint8_t messageType;
    uint8_t watchdogTimer;

    uint8_t dataBuffer[PAYLOAD_MAX_SIZE];
}Robot;


void Error_Handler(void);
extern bool isUsbEnumerated;

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H */