
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTORS_H
#define __MOTORS_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "utilities.h"

void initMotors();
void setMotor1(int8_t speed);
void setMotor2(int8_t speed);

#ifdef __cplusplus
}
#endif

#endif /* __USB_H */