
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_H
#define __USB_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "utilities.h"
#include "usbd_cdc_interface.h"

void initUSB();
bool writeUSB(Message* msg, uint8_t payloadSize);
uint8_t readUSB(uint8_t* data);
bool isUSBReadyToWrite();


#ifdef __cplusplus
}
#endif

#endif /* __USB_H */