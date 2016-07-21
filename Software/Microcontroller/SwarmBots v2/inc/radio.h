
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_H
#define __RADIO_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "utilities.h"
#include "RF24.h"

bool initRadio();
void sendRadioMessage(Message *msg, uint8_t size);
void sendAddressRequest();

void resetCommunicationWatchdog();
void tickCommunicationWatchdog(uint8_t millis);
uint32_t remainingCommunicationWatchdog();

void setRobotPipeAddress(uint64_t _pipeAddress);
void setRobotId(uint8_t _id);
uint64_t getRobotPipeAddress();
uint8_t getRobotId();

void openCommunication();

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_H */