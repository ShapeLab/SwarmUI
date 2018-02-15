
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "utilities.h"
#include "colors.h"

void initLeds();
void setRGBLed(uint8_t red, uint8_t green, uint8_t blue);

void glowRedLed();
void glowGreenLed();
void glowBlueLed();
void glowStatusLed();

void toggleRedLed();
void toggleGreenLed();
void toggleBlueLed();
void toggleStatusLed();

void blinkRedLed(bool enable);
void blinkGreenLed(bool enable);
void blinkBlueLed(bool enable);
void blinkStatusLed(bool enable);

void setRedLed(uint8_t red);
void setGreenLed(uint8_t green);
void setBlueLed(uint8_t blue);
void setStatusLed(uint8_t status);

void setRedLedRaw(uint16_t redRaw);
void setGreenLedRaw(uint16_t greenRaw);
void setBlueLedRaw(uint16_t blueRaw);
void setStatusLedRaw(uint16_t statusRaw);

void saveColors();
void restoreColors();

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */