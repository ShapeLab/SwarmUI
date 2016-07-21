
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "RingBuffer.h"

void initPhotoDiodes();
void readPhotoDiode(uint8_t photodiodeId);
bool calculatePhotodiodePosition(uint8_t photodiodeId);
void updateRobotPosition();

#ifdef __cplusplus
}
#endif

#endif /* __FUNCTIONS_H */