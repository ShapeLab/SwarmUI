
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
void selectRobot(Robot *r);

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_H */