
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POSITION_CONTROL_H
#define __POSITION_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h" 
#include "utilities.h"
#include "sensors.h"
#include "motors.h"

#define StartPosition 0
#define Orient 1
#define GoStraight 2
#define Stop 3
#define Pause 4
#define StopBeforeAdjust 5
#define Adjust 6
//Finding Minimum Velocity
#define SetMotor 7
#define IncreaseSpeed 8
#define FoundMinVel 9
//Finding Preferred Velocity (430 mm / sec)
#define OrientPrefVel 10
#define SetMotorPrefVel 11
#define IncreaseSpeedPrefVel 12
#define FoundPrefVel 13
//Accelerate, constant, decelerate
#define Accelerate 14
#define Decelerate 15

bool calibrate(Motor* motorValues);
void angleControl(float angle_desired, Motor* motorValue, bool* aligned, bool forward, float angleThreshold);
void positionControl(int xTarget, int yTarget, float finalAngle, Motor* motorValues, bool* reached, bool forward, bool finalGoal);
void maximum(int* value, int max);
void minimum(int* value, int min);
void minimumf(float* value, float min);
void maximumf(float* value, float max);
void minimumc(int8_t* value, int8_t min);
void maximumc(int8_t* value, int8_t max);
bool increaseVelocity(int8_t* velocity, Motor* motorValue, uint32_t timeIncrement);
bool decreaseVelocity(int8_t* velocity, Motor* motorValue, uint32_t timeIncrement);
#ifdef __cplusplus
}
#endif

#endif /* __LED_H */