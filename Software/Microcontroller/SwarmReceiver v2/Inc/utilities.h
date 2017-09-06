
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UTILITIES_H
#define __UTILITIES_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h" 

void initMicroTimer();
uint64_t getMicroSeconds();
void delayMicroseconds(uint32_t micros);

bool SPIInit();
uint8_t SPIWriteRegisters(uint8_t registerAddress, uint8_t length, uint8_t* data);
uint8_t SPIWriteRegister(uint8_t registerAddress, uint8_t data);
uint8_t SPIWriteCommand(uint8_t cmd);
uint8_t SPIReadBytes(uint8_t registerAddress, uint8_t length, uint8_t* data);
uint8_t SPIReadByte(uint8_t registerAddress, uint8_t* data);

void SystemClock_Config(void);
static void SystemClockHSI48_Config(void);
static void SystemClockHSE_Config(void);

void Error_Handler(void);
void assert_failed(uint8_t *file, uint32_t line);
void Reboot();

#ifdef __cplusplus
}
#endif

#endif /* __UTILITIES_H */