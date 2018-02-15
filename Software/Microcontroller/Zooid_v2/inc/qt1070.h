/*******************************************************************************
*   $FILE:  QT1070.h
*   Brief: Header file for the AT42QT1070 device and it consists AT42QT1070
*           Setup Block structure and address map enums
*   Atmel Corporation:  http:/www.atmel.com
*   Support email:  touch@atmel.com
******************************************************************************/
/*  License
*   Copyright (c) 2012, Atmel Corporation All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions are met:
*
*   1. Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
*   2. Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
*   3. The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
*   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
*   SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
*   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __QT1070_H
#define __QT1070_H

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================
Include files
============================================================================*/
#include "config.h"
#include "utilities.h"

/*============================================================================
Macros
============================================================================*/

// Chip ID (read from device-address 0)
#define QT_DEVICE_ID 0x2E

// I2C address
#define QT_ADDRESS 0x1B << 1 | 1

// Setup block start address
#define QT_SETUPS_BLOCK_ADDR 32

// detection status and key status start address
#define QT_STATUS_ADDR 2

#define QT_CHIP_ID 0
#define QT_FIRMWARE_VERSION 1
#define QT_DETECTION_STATUS 2
#define QT_KEY_STATUS 3
#define QT_KEY0_SIGNAL 4
#define QT_KEY1_SIGNAL 6
#define QT_KEY2_SIGNAL 8
#define QT_KEY3_SIGNAL 10
#define QT_KEY4_SIGNAL 12
#define QT_KEY5_SIGNAL 14
#define QT_KEY6_SIGNAL 16
#define QT_KEY0_REFERENCE 18
#define QT_KEY1_REFERENCE 20
#define QT_KEY2_REFERENCE 22
#define QT_KEY3_REFERENCE 24
#define QT_KEY4_REFERENCE 26
#define QT_KEY5_REFERENCE 28
#define QT_KEY6_REFERENCE 30
#define QT_KEY0_NTHR 32
#define QT_KEY1_NTHR 33
#define QT_KEY2_NTHR 34
#define QT_KEY3_NTHR 35
#define QT_KEY4_NTHR 36
#define QT_KEY5_NTHR 37
#define QT_KEY6_NTHR 38
#define QT_KEY0__AVE_AKS 39
#define QT_KEY1_AVE_AKS 40
#define QT_KEY2_AVE_AKS 41
#define QT_KEY3_AVE_AKS 42
#define QT_KEY4_AVE_AKS 43
#define QT_KEY5_AVE_AKS 44
#define QT_KEY6_AVE_AKS 45
#define QT_KEY0_DI 46
#define QT_KEY1_DI 47
#define QT_KEY2_DI 48
#define QT_KEY3_DI 49
#define QT_KEY4_DI 50
#define QT_KEY5_DI 51
#define QT_KEY6_DI 52
#define QT_FO_MO_GDNO 53
#define QT_LP 54
#define QT_MAX_ON_DURATION 55
#define QT_CALIBRATE 56
#define QT_RESET 57

/*
application storage for QT1070 device-status

QtStatus[0] - Detection Status
				Bit0 - TOUCH : This bit is set if any keys are in detect.
				Bit6 - OVERFLOW : This bit is set if the time to acquire all
                key signals exceeds 8ms
				bit7 - CALIBRATE : This bit is set during a calibrate sequence

QtStatus[1] - Key Status
				bits 0 - 6 : Indicates which keys are in detection
                  (Touched keys report as 1)
*/

/*============================================================================
Type definitions
============================================================================*/

typedef struct
{
    // Negative Threshold(NTHR)
    uint8_t key0_NTHR; // Key0 Negative Threshold
    uint8_t key1_NTHR; // Key1 Negative Threshold
    uint8_t key2_NTHR; // Key2 Negative Threshold
    uint8_t key3_NTHR; // Key3 Negative Threshold
    uint8_t key4_NTHR; // Key4 Negative Threshold
    uint8_t key5_NTHR; // Key5 Negative Threshold
    uint8_t key6_NTHR; // Key6 Negative Threshold

    // Averaging factor(AVE)/Adjucent key suppression level(AKS)
    uint8_t key0_AVE_AKS; // Key0 AKS group
    uint8_t key1_AVE_AKS; // Key1 AKS group
    uint8_t key2_AVE_AKS; // Key2 AKS group
    uint8_t key3_AVE_AKS; // Key3 AKS group
    uint8_t key4_AVE_AKS; // Key4 AKS group
    uint8_t key5_AVE_AKS; // Key5 AKS group
    uint8_t key6_AVE_AKS; // Key6 AKS group

    // Detection Integrator(DI)
    int8_t key0_DI; // Key0 Detection Integrator counter
    int8_t key1_DI; // Key1 Detection Integrator counter
    int8_t key2_DI; // Key2 Detection Integrator counter
    int8_t key3_DI; // Key3 Detection Integrator counter
    int8_t key4_DI; // Key4 Detection Integrator counter
    int8_t key5_DI; // Key5 Detection Integrator counter
    int8_t key6_DI; // Key6 Detection Integrator counter

    // Fast out DI, max cal, Guard channel
    uint8_t FastOutDI_MaxCal_GuardChannel;
    // 8ms intervals between key measurements
    uint8_t LP_Mode;

    // Max On Duaration
    uint8_t Max_On_Dur;

} SetupBlock;

/*============================================================================
Prototypes
============================================================================*/
bool initTouch();
bool readSetupBlock(uint8_t ReadLength, uint8_t *ReadPtr);
bool writeSetupBlock();
uint8_t readTouchKeyStatus();
uint16_t readTouchKeySignal(uint8_t key);
void resetTouch();
uint8_t getTouchDeviceID();
void calibrateTouch();

#ifdef __cplusplus
}
#endif

#endif /* __QT1070_H */
/*============================================================================
  END OF FILE
============================================================================*/