
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "utilities.h"
#include "stm32f0xx_lp_modes.h"
#include "led.h"
#include "motors.h"
#include "radio.h"
#include "sensors.h"
#include "position_control.h"
#include "eeprom.h"

void initRobot();
void updateRobot();
void sleep();
void checkRadio();
void handleIncomingRadioMessage();
void prepareStatusToSend(Position *position, float *orientation, uint8_t touch, uint8_t battery);
void sendAddressRequest();
void checkTouch();
void checkCharger();
void checkIMU();
void switchToChargingMode();

void saveToMemory();
void readFromMemory();

#ifdef __cplusplus
}
#endif
#endif /* __FUNCTIONS_H */