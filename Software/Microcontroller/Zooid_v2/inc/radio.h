
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

bool initRadio(uint8_t id);
void sendRadioMessage(Message *msg, uint8_t size);

void setRobotPipeAddress(uint64_t _pipeAddress);
void setRobotId(uint8_t _id);
uint64_t getRobotPipeAddress();
uint8_t getRobotId();

void openCommunication();
void prepareAnswer(Message *msg, uint8_t size);
uint8_t getLastMessageType();

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_H */