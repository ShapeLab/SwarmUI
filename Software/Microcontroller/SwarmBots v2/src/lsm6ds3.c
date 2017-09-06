/******************************************************************************
SparkFunLSM6DS3.cpp
LSM6DS3 Arduino and Teensy Driver

Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/LSM6DS3_Breakout
https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation
Either can be omitted if not used

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Please review the LICENSE.md file included with this example. If you have any questions
or concerns with licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.
******************************************************************************/

//See SparkFunLSM6DS3.h for additional topology notes.

#include "lsm6ds3.h"

uint8_t LSM6DS3_ADDRESS = 0b11010101;
uint8_t IMUBuffer[22] = {0};

GyroRawData gyroOffsets;
MotionData motionData;

static const float LSM6DS3_FS_XL_SENSITIVITY[] = {0.000061f, 0.000488f, 0.000122f, 0.000244f};
static const float LSM6DS3_FS_G_SENSITIVITY[] = {0.00875f, 0.0175f, 0.035f, 0.07f};

LSM6DS3_FS_XL_t currentFSXL;
LSM6DS3_FS_G_t currentFSG;

bool initImu()
{
    if (getImuId() != LSM6DS3_DEVICE_ID)
    {
        LSM6DS3_ADDRESS = 0b11010111;
        if (getImuId() != LSM6DS3_DEVICE_ID)
        {
            if (DEBUG_ENABLED())
                debug_printf("IMU not detected\n");
            return false;
        }
    }

    resetImu();
    initAccel();
    initGyro();
    calibrateImu();

    enableSingleTap();
    initInterrupts();
    if (DEBUG_ENABLED())
        debug_printf("IMU successfully initialized and calibrated\n");

   
    return true;
}

uint8_t getImuId()
{
    if(I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_WHO_AM_I_REG, IMUBuffer))
      return IMUBuffer[0];
    else
      return 0xff;
}

bool resetImu()
{
    if (I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL3_C, LSM6DS3_SW_RESET_RESET_DEVICE) == true)
    {
        HAL_Delay(50);
        return true;
    }
    else
        return false;
}

bool calibrateImu()
{
    uint16_t i = 0;
    GyroRawData rotation;

    gyroOffsets.gx = gyroOffsets.gy = gyroOffsets.gz = 0;

    if (DEBUG_ENABLED())
        debug_printf("Gyro Calibration, collecting measurements...\n");

    for (i = 0; i < 1000; i += 10)
    {
        getRawRotation(&rotation.gx, &rotation.gy, &rotation.gz);

        if (i == 0)
        {
            gyroOffsets.gx = rotation.gx;
            gyroOffsets.gy = rotation.gy;
            gyroOffsets.gz = rotation.gz;
        }
        else
        {
            gyroOffsets.gx = (gyroOffsets.gx + rotation.gx) / 2;
            gyroOffsets.gy = (gyroOffsets.gy + rotation.gy) / 2;
            gyroOffsets.gz = (gyroOffsets.gz + rotation.gz) / 2;
        }
        delayMicroseconds(10000);
    }
    if (DEBUG_ENABLED())
        debug_printf("Gyro Offsets : gx=%d | gy=%d | gz=%d\n", gyroOffsets.gx, gyroOffsets.gy, gyroOffsets.gz);
}

bool getMotionT(MotionData *motionData)
{
    ImuRawData imuData;

    bool s = getRawMotionT(&imuData);

    motionData->T = calculateTemperature(imuData.T);
    motionData->ax = calculateAcceleration(imuData.ax);
    motionData->ay = calculateAcceleration(imuData.ay);
    motionData->az = calculateAcceleration(imuData.az);
    motionData->gx = calculateRotation(imuData.gx - gyroOffsets.gx);
    motionData->gy = calculateRotation(imuData.gy - gyroOffsets.gy);
    motionData->gz = calculateRotation(imuData.gz - gyroOffsets.gz);

    return s;
}

float calculateTemperature(int16_t rawInput)
{
    return ((float)rawInput / 16.0f + 25.0f);
}

float calculateAcceleration(int16_t rawInput)
{
    float tmp = (float)rawInput;
    tmp *= LSM6DS3_FS_XL_SENSITIVITY[currentFSXL];
    return tmp; //(float)rawInput *  LSM6DS3_FS_XL_SENSITIVITY[currentFSXL];
}

float calculateRotation(int16_t rawInput)
{
    float tmp = (float)rawInput;
    tmp *= LSM6DS3_FS_G_SENSITIVITY[currentFSG];
    return tmp; // (float)rawInput * LSM6DS3_FS_G_SENSITIVITY[currentFSG];
}

bool getRawMotionT(ImuRawData *imu)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUT_TEMP_L, 14, IMUBuffer);

    imu->T = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);

    imu->gx = ((((int16_t)IMUBuffer[3]) << 8) | IMUBuffer[2]);
    imu->gy = ((((int16_t)IMUBuffer[5]) << 8) | IMUBuffer[4]);
    imu->gz = ((((int16_t)IMUBuffer[7]) << 8) | IMUBuffer[6]);

    imu->ax = ((((int16_t)IMUBuffer[9]) << 8) | IMUBuffer[8]);
    imu->ay = ((((int16_t)IMUBuffer[11]) << 8) | IMUBuffer[10]);
    imu->az = ((((int16_t)IMUBuffer[13]) << 8) | IMUBuffer[12]);

    return b == true ? true : false;
}

bool getRawAcceleration(int16_t *ax, int16_t *ay, int16_t *az)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTX_L_XL, 6, IMUBuffer);
    *ax = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    *ay = ((((int16_t)IMUBuffer[3]) << 8) | IMUBuffer[2]);
    *az = ((((int16_t)IMUBuffer[5]) << 8) | IMUBuffer[4]);

    return b == true ? true : false;
}

bool getRawRotation(int16_t *gx, int16_t *gy, int16_t *gz)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTX_L_G, 6, IMUBuffer);
    *gx = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    *gy = ((((int16_t)IMUBuffer[3]) << 8) | IMUBuffer[2]);
    *gz = ((((int16_t)IMUBuffer[5]) << 8) | IMUBuffer[4]);

    return b == true ? true : false;
}

bool getRawAccelerationX(int16_t *ax)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTX_L_XL, 2, IMUBuffer);
    *ax = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawAccelerationY(int16_t *ay)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTY_L_XL, 2, IMUBuffer);
    *ay = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawAccelerationZ(int16_t *az)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTZ_L_XL, 2, IMUBuffer);
    *az = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawTemperature(int16_t *t)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUT_TEMP_L, 2, IMUBuffer);
    *t = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawRotationX(int16_t *gx)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTX_L_G, 2, IMUBuffer);
    *gx = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawRotationY(int16_t *gy)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTY_L_G, 2, IMUBuffer);
    *gy = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool getRawRotationZ(int16_t *gz)
{
    bool b = I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_OUTZ_L_G, 2, IMUBuffer);
    *gz = ((((int16_t)IMUBuffer[1]) << 8) | IMUBuffer[0]);
    return b == true ? true : false;
}

bool initGyro()
{
    currentFSG = LSM6DS3_FS_G_245dps;

    setGyroRange(currentFSG);
    setGyroODR(LSM6DS3_ODR_G_416Hz);
}

bool initAccel()
{
    currentFSXL = LSM6DS3_FS_XL_2g;

    setAccelRange(currentFSXL);
    setAccelBW(LSM6DS3_BW_XL_400Hz);
    setAccelODR(LSM6DS3_ODR_XL_416Hz);
}

bool initInterrupts()
{
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_MD1_CFG, LSM6DS3_INT1_6D_ENABLED |
                                                   LSM6DS3_INT1_SINGLE_TAP_DISABLED |
                                                   LSM6DS3_INT1_TILT_ENABLED);     // Interrupt driven to INT1 pin
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_MD2_CFG, 0x00);     // No interrupt on INT2 pin since not wired
}

//{wake_up,tap,d6d,func}
void getImuInterruptSource(uint8_t *interruptSrc)
{
    I2CReadBytes(LSM6DS3_ADDRESS, LSM6DS3_WAKE_UP_SRC, 3, interruptSrc);
    I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_FUNC_SRC, &interruptSrc[3]);
}
// get int source, read WAKE_UP_SRC, D6D_SRC, TAP_SRC and FUNC_SRC.

bool setAccelRange(LSM6DS3_FS_XL_t range)
{
    uint8_t tmpReg = 0;
    if (I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, &tmpReg) == true)
    {
        tmpReg |= range;
        return I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, tmpReg);
    }
    return false;
}

bool setAccelBW(LSM6DS3_BW_XL_t bw)
{
    uint8_t tmpReg = 0;
    if (I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, &tmpReg) == true)
    {
        tmpReg |= bw;
        return I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, tmpReg);
    }
    return false;
}

bool setAccelODR(LSM6DS3_ODR_XL_t rate)
{
    uint8_t tmpReg = 0;
    if (I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, &tmpReg) == true)
    {
        tmpReg |= rate;
        return I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL1_XL, tmpReg);
    } 
    return false;
}

bool setGyroODR(LSM6DS3_ODR_G_t rate)
{
    uint8_t tmpReg = 0;
    if (I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL2_G, &tmpReg) == true)
    {
        tmpReg |= rate;
        return I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL2_G, tmpReg);
    }
    return false;
}

bool setGyroRange(LSM6DS3_FS_G_t range)
{
    uint8_t tmpReg = 0;
    if (I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL2_G, &tmpReg) == true)
    {
        tmpReg |= range;
        return I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_CTRL2_G, tmpReg);
    }
    return false;
}

bool enableSingleTap()
{
    uint8_t tmpReg = 0;

    //configure TAP_CFG for single tap on the top of the zooid
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_TAP_CFG1, LSM6DS3_TAP_Z_EN_ENABLED);  

    //setting thresholds and tap duration
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_TAP_THS_6D, 0x09);
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_INT_DUR2, 0x06);

    //enabling tap detection
    I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_WAKE_UP_THS, &tmpReg);
    tmpReg |= LSM6DS3_SINGLE_DOUBLE_TAP_SINGLE_TAP;
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_WAKE_UP_THS, tmpReg);

    //enabling double tap interrupt
    I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_MD1_CFG, &tmpReg);
    tmpReg |= LSM6DS3_INT1_SINGLE_TAP_ENABLED;
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_MD1_CFG, tmpReg);
}

bool enableDoubleTap()
{
    uint8_t tmpReg = 0;

    //configure TAP_CFG for double tap on the top of the zooid
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_TAP_CFG1, LSM6DS3_TAP_Z_EN_ENABLED | LSM6DS3_LIR_ENABLED);  

    //setting thresholds and tap duration
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_TAP_THS_6D, 0x0C);
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_INT_DUR2, 0x7F);

    //enabling double tap detection
    I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_WAKE_UP_THS, &tmpReg);
    tmpReg |= LSM6DS3_SINGLE_DOUBLE_TAP_DOUBLE_TAP;
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_WAKE_UP_THS, tmpReg);

    //enabling double tap interrupt
    I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_MD1_CFG, &tmpReg);
    tmpReg |= LSM6DS3_INT1_DOUBLE_TAP_ENABLED;
    I2CWriteByte(LSM6DS3_ADDRESS, LSM6DS3_MD1_CFG, tmpReg);
}

uint8_t getTapSource()
{ 
    if(I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_TAP_SRC, IMUBuffer))
      return IMUBuffer[0];
    else
      return 0xff;
}

uint8_t getD6DSource()
{
    if(I2CReadByte(LSM6DS3_ADDRESS, LSM6DS3_D6D_SRC, IMUBuffer))
      return IMUBuffer[0];
    else
      return 0xff;
}

