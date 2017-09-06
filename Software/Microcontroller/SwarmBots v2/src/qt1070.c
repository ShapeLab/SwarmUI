/******************************************************************************* 
*   $FILE:  QT_I2C.c
*   Brief: Application interfaces to drive the QT device 
*   Atmel Corporation:  http://www.atmel.com
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

/*============================================================================
Include files
============================================================================*/
#include "qt1070.h"

/*============================================================================
Global variable definitions
============================================================================*/
uint8_t QTBuffer[22] = {0};
/*============================================================================
Function Definitions
============================================================================*/
/*============================================================================
Name    :   InitQtInterface
------------------------------------------------------------------------------
Purpose :	Initialize communication interface and change notification pin
		and reset pin
Input   :   none
Output  :   none
Return	:   none
Notes   :
============================================================================*/
bool initTouch()
{
    if (getTouchDeviceID() == QT_DEVICE_ID)
    {
        resetTouch();
        HAL_Delay(250);
        writeSetupBlock();
        calibrateTouch();
        if (DEBUG_ENABLED())
            debug_printf("Touch sensor successfully initialized\n");
        return true;
    }
    else
    {
        if (DEBUG_ENABLED())
            debug_printf("Touch sensor not detected\n");
        return false;
    }
}

/*============================================================================
Name    :   ReadSetupBlock
------------------------------------------------------------------------------
Purpose :	Read entire setup block from QT-device
Input   :   ReadLength	:	Number of bytes to read
			ReadPtr		:	Pointer to byte array for read-data
Output  :   none
Return	:	TRUE if read successful, else FALSE
Notes   :
============================================================================*/
bool readSetupBlock(uint8_t ReadLength, uint8_t *ReadPtr)
{
    // Read setup block
    return true;
}

/*============================================================================
Name    :   WriteSetupBlock
------------------------------------------------------------------------------
Purpose :	write entire setup block to QT-device
Input   :   WriteLength	:	Number of bytes to write
			WritePtr	:	Pointer to byte array containing write-data
Output  :   none
Return	:	TRUE if write successful, else FALSE
Notes   :
============================================================================*/
bool writeSetupBlock()
{
    SetupBlock setup_block;

    setup_block.key0_NTHR =
        setup_block.key1_NTHR =
            setup_block.key2_NTHR =
                setup_block.key3_NTHR =
                    setup_block.key4_NTHR =
                        setup_block.key5_NTHR =
                            setup_block.key6_NTHR = 25;//5

    setup_block.key0_AVE_AKS =
        setup_block.key1_AVE_AKS =
            setup_block.key2_AVE_AKS =
                setup_block.key3_AVE_AKS =
                    setup_block.key4_AVE_AKS =
                        setup_block.key5_AVE_AKS = 0x10;

    setup_block.key6_AVE_AKS = 0;

    setup_block.key0_DI =
        setup_block.key1_DI =
            setup_block.key2_DI =
                setup_block.key3_DI =
                    setup_block.key4_DI =
                        setup_block.key5_DI =
                            setup_block.key6_DI = 4;

    setup_block.FastOutDI_MaxCal_GuardChannel = 0x17; //No guard

    setup_block.LP_Mode = 8; // 8ms * 1 = 32ms between key measurements
    setup_block.Max_On_Dur = 63;//255;
    bool b = I2CWriteBytes(QT_ADDRESS, QT_SETUPS_BLOCK_ADDR, sizeof(setup_block), (uint8_t *)&setup_block);

    return b;
}

/*============================================================================
Name    :   readTouchKeyStatus
------------------------------------------------------------------------------
Purpose :	Read detection status of all keys
Input   :   
Output  :     bitmap of the keys' statuses
Return	:	
Notes   :
============================================================================*/
uint8_t readTouchKeyStatus()
{
    // Read detection status of all keys
    I2CReadBytes(QT_ADDRESS, QT_DETECTION_STATUS, 2, QTBuffer);
    return QTBuffer[1];
}

/*============================================================================
Name    :   readKeySignal
------------------------------------------------------------------------------
Purpose :   Reads the raw signals for the given key
Input   :   none
Output  :   none
Return	:   none
Notes   :
============================================================================*/
uint16_t readTouchKeySignal(uint8_t key)
{
    I2CReadBytes(QT_ADDRESS, QT_KEY0_SIGNAL + key * 2, 2, QTBuffer);
    return ((uint16_t)QTBuffer[0]) << 8 | QTBuffer[1];
}

/*============================================================================
Name    :   ResetQT
------------------------------------------------------------------------------
Purpose :   Performs a hardware reset of the QT device
Input   :   none
Output  :   none
Return	:   none
Notes   :
============================================================================*/
void resetTouch()
{
    HAL_Delay(10);
    I2CWriteByte(QT_ADDRESS, QT_RESET, 0xFF);
}

/*============================================================================
Name    :   CalibrateQT
------------------------------------------------------------------------------
Purpose :   triggers the device to start a calibration cycle
Input   :   none
Output  :   none
Return	:   none
Notes   :
============================================================================*/
void calibrateTouch()
{
    uint8_t calibrating = 0;
    I2CWriteByte(QT_ADDRESS, QT_CALIBRATE, 0xFF);
    do
    {
        I2CReadBit(QT_ADDRESS, QT_DETECTION_STATUS, 7, &calibrating);
    } while (calibrating > 0);
}

/*============================================================================
Name    :   getQTDeviceID
------------------------------------------------------------------------------
Purpose :   Check communication is ready and able to read Chip ID
Input   :   none
Output  :   none
Return	:   none
Notes   :
============================================================================*/
uint8_t getTouchDeviceID()
{
    I2CReadByte(QT_ADDRESS, QT_CHIP_ID, QTBuffer);

    return QTBuffer[0];
}