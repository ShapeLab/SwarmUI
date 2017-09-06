
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "utilities.h"
#include "colors.h"

void initRGBLed();
void setRGBLed(uint8_t red, uint8_t green, uint8_t blue);
void setColor(const uint8_t *color);

void glowRedLed();
void glowGreenLed();
void glowBlueLed();

void toggleRedLed();
void toggleGreenLed();
void toggleBlueLed();

void setRedLed(uint8_t red);
void setGreenLed(uint8_t green);
void setBlueLed(uint8_t blue);

void setRedLedRaw(uint16_t redRaw);
void setGreenLedRaw(uint16_t greenRaw);
void setBlueLedRaw(uint16_t blueRaw);

void saveColors();
void restoreColors();
#ifdef __cplusplus
}
#endif
#endif /* __LED_H */