
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORS_H
#define __SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "utilities.h"
#include "qt1070.h"
#include "lsm6ds3.h"
#include "MadgwickAHRS.h"
#include "RingBuffer.h"

bool initSensors();
void initPhotoDiodes();
void readPhotoDiode(uint8_t photodiodeId);
bool calculatePhotodiodePosition(uint8_t photodiodeId);
bool updateRobotPosition();
Position *getRobotPosition();
float *getRobotAngle();
void initBatteryMonitor();
void startReadBatteryLevel();
uint8_t getBatteryLevel();
bool isCharging();
void updateMotionInformation();
uint8_t getInteractionState();
bool isBlinded();
bool isTouched();
bool updateTouch();
void updateOrientation();
void updateTap();



void initCaseReader();
uint8_t readCaseId();


#ifdef __cplusplus
}
#endif

#endif /* __SENSORS_H */