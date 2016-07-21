
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "utilities.h"
#include "RobotRingBuffer.h"
#include "RingBuffer.h"

#include "led.h"
#include "usb.h"
#include "radio.h"

void initReceiver();
void checkRadio();
void handleIncomingRadioMessage();
void sendRobotUpdate(uint8_t robotId);

void checkUSB();
void handleIncomingUSBMessage();
void assignNewRobot();

void tickRobotWatchdogTimer(uint8_t robotId);
void resetRobotWatchdogTimer(uint8_t robotId);
#ifdef __cplusplus
}
#endif

#endif /* __FUNCTIONS_H */