
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UTILITIES_H
#define __UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"
void initMicroTimer();
uint64_t getMicroSeconds();
void delayMicroseconds(uint32_t micros);

bool I2CInit();
bool I2CWriteBytes(uint8_t sensorAddress, uint8_t registerAddress, uint8_t length, uint8_t *data);
bool I2CWriteByte(uint8_t sensorAddress, uint8_t registerAddress, uint8_t data);
bool I2CWriteBit(uint8_t sensorAddress, uint8_t registerAddress, uint8_t bitNumber, bool data);
bool I2CReadBytes(uint8_t sensorAddress, uint8_t registerAddress, uint8_t length, uint8_t *data);
bool I2CReadByte(uint8_t sensorAddress, uint8_t registerAddress, uint8_t *data);
bool I2CReadBit(uint8_t sensorAddress, uint8_t registerAddress, uint8_t bitNumber, bool *data);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);

bool SPIInit();
bool SPIFlushRxFifo();
uint8_t SPIWriteRegisters(uint8_t registerAddress, uint8_t length, uint8_t *data);
uint8_t SPIWriteRegister(uint8_t registerAddress, uint8_t data);
uint8_t SPIWriteCommand(uint8_t cmd, uint8_t *value);
uint8_t SPIReadBytes(uint8_t registerAddress, uint8_t length, uint8_t *data);
uint8_t SPIReadByte(uint8_t registerAddress, uint8_t *data);

void SystemClock_Config(void);
static void SystemClockHSE_Config(void);

void Error_Handler(void);
void assert_failed(uint8_t *file, uint32_t line);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *I2cHandle);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *I2cHandle);
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
void Reboot();

#ifdef __cplusplus
}
#endif

#endif /* __UTILITIES_H */