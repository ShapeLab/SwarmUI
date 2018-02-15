
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "utilities.h"
#include "RobotRingBuffer.h"
#include "RingBuffer.h"

#include "led.h"
#include "usb.h"
#include "radio.h"

void initReceiver();
void configureReceiver();
void checkRadio();
void handleIncomingRadioMessage();
void sendRobotUpdate(uint8_t robotId);
void restartPolling();

void checkUSB();
void handleIncomingUSBMessage();
void processUSBData();
void sendUSBMessage(Message *msg, uint8_t payloadSize);

#ifdef __cplusplus
}
#endif

#endif /* __FUNCTIONS_H */