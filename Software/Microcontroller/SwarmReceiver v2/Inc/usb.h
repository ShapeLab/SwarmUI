
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "utilities.h"


bool initUSB();
bool writeUSB(uint16_t length, uint8_t* data);
uint8_t readUSB(uint8_t* data);
bool isUSBReadyToWrite();


#ifdef __cplusplus
}
#endif

#endif /* __USB_H */