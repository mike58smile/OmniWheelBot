/*****************************************************************//**
 * \file   Gyro.cpp
 * \brief  Gyro	class source file
 * \details For sensing and processing the data from accelerometer/gyro MPU6050
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#include "Gyro.h"


void GyroClass::init()
{
    // Initialize MPU6050
    Serial.println("Initialize MPU6050");
    while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
    {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
    }

    // If you want, you can set gyroscope offsets
    // mpu.setGyroOffsetX(155);
    // mpu.setGyroOffsetY(15);
    // mpu.setGyroOffsetZ(15);

    // Calibrate gyroscope. The calibration must be at rest.
    // If you don't want calibrate, comment this line.
    mpu.calibrateGyro();

    // Set threshold sensivty. Default 3.
    // If you don't want use threshold, comment this line or set 0.
    mpu.setThreshold(3);

    // Check settings
    checkSettings();
}


void GyroClass::checkSettings()
{
    Serial.println();

    Serial.print(" * Sleep Mode:        ");
    Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

    Serial.print(" * Clock Source:      ");
    switch (mpu.getClockSource())
    {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
    }

    Serial.print(" * Gyroscope:         ");
    switch (mpu.getScale())
    {
    case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
    case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
    case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
    case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
    }

    Serial.print(" * Gyroscope offsets: ");
    Serial.print(mpu.getGyroOffsetX());
    Serial.print(" / ");
    Serial.print(mpu.getGyroOffsetY());
    Serial.print(" / ");
    Serial.println(mpu.getGyroOffsetZ());

    Serial.println();
}

float GyroClass::read(bool raw = 0)
{
    float sens = ((raw) ? Gyro.mpu.readRawGyro().ZAxis : Gyro.mpu.readNormalizeGyro().ZAxis)* 0.01;
    //ADCFilter.Filter(sens);
    //Serial.println(sens);
    return sens;
}
GyroClass Gyro{};

