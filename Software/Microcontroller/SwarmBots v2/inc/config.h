
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"
#include <string.h>
#include <math.h>
#include <__cross_studio_io.h>
#include <stdlib.h>

/* Exported macro ------------------------------------------------------------*/
#define BIT(x) (1 << (x))
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define DEBUG_ENABLED() (DEBUG)

/*
  CONSTANTS DECLARATIONS
*/

#define USE_EXTERNAL_PHOTODIODES
#define USE_FAST_MOTORS
//#define USE_SIMULATOR

///////////////////////////////////
//     OPERATING CONSTANTS       //
///////////////////////////////////
#define PI                                      3.14159265358979323846f
#define RAD_TO_DEG_CST                          (180.0f / PI)
#define DEG_TO_RAD_CST                          (PI / 180.0f)
  
#define FALSE                                   0
#define TRUE                                    1
#define LOW                                     GPIO_PIN_RESET
#define HIGH                                    GPIO_PIN_SET

#define ON                                      TRUE
#define OFF                                     FALSE
#define false                                   FALSE
#define true                                    TRUE


///////////////////////////////////
//   OPTICAL POSITIONING FRAME   //
///////////////////////////////////
//   | start |     columns    |      rows       |       end       |
//   --------------------------------------------------------------
//   |   b0  | b1 | ... | b10 | b11 | ... | b20 | b21 | ... | b40 |
//   --------------------------------------------------------------
//   |   B   | b1 | ... | b10 | b11 | ... | b20 |  W  |  W  |  W  |
#define SIZEOF_START                            1
#define SIZEOF_COLUMN                           10
#define SIZEOF_ROW                              10
#define SIZEOF_END                              20 // number of white bits in the end pattern
#define SIZEOF_PATTERN                          41

#define NB_PHOTODIODES                          2 // number of photodiodes
#define BIT_DURATION                            333 // Exposure time in microsec
#define BLACK                                   0
#define WHITE                                   1

///////////////////////////////////
//    COMMUNICATION CONSTANTS    //
///////////////////////////////////
#define RADIO_CHANNEL                           42//22 //88 //channel at which you want to communicate through
#define PAYLOAD_MAX_SIZE                        32
#define DEFAULT_ADDRESS                         0xF0F0F0F0D000
#define DEFAULT_ID                              0xFF
#define COMMUNICATION_TIMEOUT                   4000 //in ms

#define NB_MAX_ROBOTS                           10
#define RECEIVER_ID                             250

///////////////////////////////////
//      MESSAGE TYPES            //
///////////////////////////////////
//Operating messages
#define TYPE_UPDATE                             0x03
#define TYPE_STATUS                             0x04
#define TYPE_MOTORS_VELOCITY                    0x05
#define TYPE_ROBOT_POSITION                     0x06

//Configuration Messages
#define TYPE_NEW_ROBOT_REQUEST                  0xF0
#define TYPE_NEW_ROBOT_INFORMATION              0xF1

#define TYPE_REBOOT_ROBOT 0xFF

///////////////////////////////////
//  LED DECLARATIONS             //
///////////////////////////////////
#define RGB_GPIO_PORT                           GPIOA
#define RGB_GPIO_AF                             GPIO_AF2_TIM1
#define RGB_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOA_CLK_ENABLE()
#define RED_PIN                                 GPIO_PIN_10
#define GREEN_PIN                               GPIO_PIN_11
#define BLUE_PIN                                GPIO_PIN_9

#define RGB_TIM                                 TIM1
#define RGB_FREQ                                200
#define RGB_CLK                                 2000000
#define RGB_PERIOD                              (uint32_t)(RGB_CLK / RGB_FREQ) /* Period Value  */
#define RED_CHANNEL                             TIM_CHANNEL_3
#define GREEN_CHANNEL                           TIM_CHANNEL_4
#define BLUE_CHANNEL                            TIM_CHANNEL_2
#define RGB_TIMER_CLK_ENABLE()                  __HAL_RCC_TIM1_CLK_ENABLE()

///////////////////////////////////
//  MOTORS DECLARATIONS          //
///////////////////////////////////
#ifdef USE_FAST_MOTORS
#define MAX_SPEED                              60
#else
#define MAX_SPEED                               100
#endif

#define MOTOR1_PIN                              GPIO_PIN_0
#define MOTOR1_GPIO_PORT                        GPIOA
#define MOTOR1_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()

#define MOTOR2_PIN                              GPIO_PIN_1
#define MOTOR2_GPIO_PORT                        GPIOA
#define MOTOR2_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()

#define MOTOR3_PIN                              GPIO_PIN_2
#define MOTOR3_GPIO_PORT                        GPIOA
#define MOTOR3_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()

#define MOTOR4_PIN                              GPIO_PIN_3
#define MOTOR4_GPIO_PORT                        GPIOA
#define MOTOR4_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE()

#define MOTORS_TIM                              TIM2
#define MOTORS_GPIO_AF                          GPIO_AF2_TIM2
#define MOTORS_FREQ                             40000
#define MOTORS_CLK                              4000000
#define MOTORS_PERIOD                           (uint32_t)(MOTORS_CLK / MOTORS_FREQ) /* Period Value  */
#define MOTOR1_CHANNEL                          TIM_CHANNEL_1
#define MOTOR2_CHANNEL                          TIM_CHANNEL_2
#define MOTOR3_CHANNEL                          TIM_CHANNEL_3
#define MOTOR4_CHANNEL                          TIM_CHANNEL_4
#define MOTORS_CLK_ENABLE()                     __HAL_RCC_TIM2_CLK_ENABLE()

///////////////////////////////////
//  INPUT PINS DECLARATIONS      //
///////////////////////////////////
#ifdef USE_EXTERNAL_PHOTODIODES
  #define PHOTODIODE_1_PIN                        GPIO_PIN_4
  #define PHOTODIODE_1_GPIO_PORT                  GPIOB
  #define PHOTODIODE_1_IRQn                       EXTI4_15_IRQn
  #define PHOTODIODE_1_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
  #define PHOTODIODE_1_IRQ_PRIORITY               0

  #define PHOTODIODE_2_PIN                        GPIO_PIN_5
  #define PHOTODIODE_2_GPIO_PORT                  GPIOB
  #define PHOTODIODE_2_IRQn                       EXTI4_15_IRQn
  #define PHOTODIODE_2_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
  #define PHOTODIODE_2_IRQ_PRIORITY               0

#else

  #define PHOTODIODE_1_PIN                        GPIO_PIN_9
  #define PHOTODIODE_1_GPIO_PORT                  GPIOB
  #define PHOTODIODE_1_IRQn                       EXTI4_15_IRQn
  #define PHOTODIODE_1_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
  #define PHOTODIODE_1_IRQ_PRIORITY               0

  #define PHOTODIODE_2_PIN                        GPIO_PIN_1
  #define PHOTODIODE_2_GPIO_PORT                  GPIOB
  #define PHOTODIODE_2_IRQn                       EXTI0_1_IRQn
  #define PHOTODIODE_2_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
  #define PHOTODIODE_2_IRQ_PRIORITY               0
#endif

#define CHARGING_STATUS_PIN                     GPIO_PIN_13
#define CHARGING_STATUS_GPIO_PORT               GPIOB
#define CHARGING_STATUS_IRQn                    EXTI4_15_IRQn
#define CHARGING_STATUS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHARGING_STATUS_IRQ_PRIORITY            3

/* Definition for QT1070 Int Pin */
#define TOUCH_CHANGE_PIN                        GPIO_PIN_2
#define TOUCH_CHANGE_GPIO_PORT                  GPIOB
#define TOUCH_CHANGE_IRQn                       EXTI2_3_IRQn
#define TOUCH_CHANGE_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define TOUCH_CHANGE_IRQ_PRIORITY               3

/* Definition for IMU Int Pin */
#define IMU_INT_PIN                             GPIO_PIN_12
#define IMU_INT_GPIO_PORT                       GPIOB
#define IMU_INT_IRQn                            EXTI4_15_IRQn
#define IMU_INT_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define IMU_INT_IRQ_PRIORITY                    3

/* Definition for Radio Int Pin */
#define RADIO_IRQ_PIN                           GPIO_PIN_15
#define RADIO_IRQ_GPIO_PORT                     GPIOA
#define RADIO_IRQn                              EXTI4_15_IRQn
#define RADIO_IRQ_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define RADIO_IRQ_PRIORITY                      1

///////////////////////////////////
//  RADIO DECLARATIONS           //
///////////////////////////////////
/* Definition for SPIx Pins */
#define SPIx_CSN_PIN                            GPIO_PIN_4
#define SPIx_CSN_GPIO_PORT                      GPIOA
#define SPIx_CSN_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_SCK_PIN                            GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT                      GPIOA
#define SPIx_SCK_AF                             GPIO_AF0_SPI1
#define SPIx_SCK_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_MISO_PIN                           GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT                     GPIOA
#define SPIx_MISO_AF                            GPIO_AF0_SPI1
#define SPIx_MISO_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_MOSI_PIN                           GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT                     GPIOA
#define SPIx_MOSI_AF                            GPIO_AF0_SPI1
#define SPIx_MOSI_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_CE_PIN                             GPIO_PIN_0
#define SPIx_CE_GPIO_PORT                       GPIOB
#define SPIx_CE_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx                                    SPI1
#define SPIx_CLK_ENABLE()                       __HAL_RCC_SPI1_CLK_ENABLE()

#define DMAx_CLK_ENABLE()                       __HAL_RCC_DMA1_CLK_ENABLE()

/* Definition for SPIx's DMA */
#define SPIx_RX_DMA_STREAM                      DMA1_Channel2
#define SPIx_TX_DMA_STREAM                      DMA1_Channel3

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                        DMA1_Channel2_3_IRQn
#define SPIx_DMA_RX_IRQn                        DMA1_Channel2_3_IRQn
#define SPIx_DMA_TX_IRQHandler                  DMA1_Channel2_3_IRQHandler
#define SPIx_DMA_RX_IRQHandler                  DMA1_Channel2_3_IRQHandler

#define SPIx_FORCE_RESET()                      __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()                    __HAL_RCC_SPI1_RELEASE_RESET()

///////////////////////////////////
//  SENSORS DECLARATIONS         // 
///////////////////////////////////
/* Definition for I2C clock resources */
#define I2Cx                                    I2C2
#define I2Cx_CLK_ENABLE()                       __HAL_RCC_I2C2_CLK_ENABLE()
#define I2Cx_TIMING                             0x00901850 //0x0010020A
#define I2Cx_FORCE_RESET()                      __HAL_RCC_I2C2_FORCE_RESET()
#define I2Cx_RELEASE_RESET()                    __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C Pins */
#define I2Cx_SCL_PIN                            GPIO_PIN_10
#define I2Cx_SCL_GPIO_PORT                      GPIOB
#define I2Cx_SCL_AF                             GPIO_AF1_I2C2
#define I2Cx_SCL_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_SDA_PIN                            GPIO_PIN_11
#define I2Cx_SDA_GPIO_PORT                      GPIOB
#define I2Cx_SDA_AF                             GPIO_AF1_I2C2
#define I2Cx_SDA_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()

/* Definition for I2Cx's DMA */
#define I2Cx_DMA                                DMA1
#define I2Cx_TX_DMA_INSTANCE                    DMA1_Channel4
#define I2Cx_RX_DMA_INSTANCE                    DMA1_Channel5
#define I2Cx_DMA_CLK_ENABLE()                   __HAL_RCC_DMA1_CLK_ENABLE()

/* Definition for I2Cx's DMA NVIC */
#define I2Cx_DMA_TX_IRQn                        DMA1_Channel4_5_IRQn
#define I2Cx_DMA_RX_IRQn                        DMA1_Channel4_5_IRQn
#define I2Cx_DMA_TX_IRQHandler                  DMA1_Channel4_5_IRQHandler
#define I2Cx_DMA_RX_IRQHandler                  DMA1_Channel4_5_IRQHandler

///////////////////////////////////
// MICROSECONDS DELAY TIMER      //
///////////////////////////////////
#define MICRO_TIM                               TIM3
#define MICRO_FREQ                              100
#define MICRO_CLK                               1000000
#define MICRO_PERIOD                            (uint32_t)(MICRO_CLK / MICRO_FREQ) /* Period Value  */
#define MICRO_TIM_CLK_ENABLE()                  __HAL_RCC_TIM3_CLK_ENABLE();
#define MICRO_IRQn                              TIM3_IRQn
#define MICRO_IRQHandler                        TIM3_IRQHandler
#define MICRO_IRQ_PRIORITY                      0


#define POWER_MGMT_CLK_ENABLE()                 __HAL_RCC_PWR_CLK_ENABLE()

///////////////////////////////////
// Zoommings                     //
///////////////////////////////////
/* Definition for QT1070 Int Pin */
#define CASE_SEL_0_PIN                          GPIO_PIN_6
#define CASE_SEL_0_GPIO_PORT                    GPIOB
#define CASE_SEL_0_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define CASE_SEL_1_PIN                          GPIO_PIN_7
#define CASE_SEL_1_GPIO_PORT                    GPIOB
#define CASE_SEL_1_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()




///////////////////////////////////
// Exported types                //
///////////////////////////////////
typedef uint8_t MotorDirection;
typedef uint8_t bool;

typedef struct {
	uint16_t positionX;
	uint16_t positionY;
	uint8_t colorRed;
	uint8_t colorGreen;
	uint8_t colorBlue;
	uint8_t preferredSpeed;
	int16_t orientation;
	bool isFinalGoal;
	uint8_t empty;
}PositionControlMessage;

typedef struct {
	uint16_t positionX;
	uint16_t positionY;
	int16_t orientation;
	uint8_t touch;
	uint8_t batteryLevel;
}StatusMessage;

typedef struct
{
    int8_t motor1;
    int8_t motor2;
    float motorGain;
    int8_t minVelocity;
    int8_t preferredVelocity;
    int8_t maxVelocity;
} Motor;

typedef struct
{
  uint16_t x;
  uint16_t y;
  float angle;
  bool finalGoal;
} Target;

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
    uint8_t payload[PAYLOAD_MAX_SIZE - sizeof(Header)];
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
  int8_t x;
  int8_t y;
  int8_t z;
} OrientationVec;

typedef struct
{
    uint8_t id;
    uint64_t pipeAddress;
} Robot;

#ifdef __cplusplus
}
#endif

#endif