/*****************************************************************//**
 * \file   Gyro.h
 * \brief  Gyro.h class 
 * \details Class sensing and processing the data from accelerometer/gyro MPU6050
 * 
 * \author xmisko06
 * \date   August 2023
 *********************************************************************/

#ifndef _GYRO_h
#define _GYRO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class GyroClass
{
 protected:
	 Adafruit_MPU6050 mpu;
 public:
	void init();
	float read();
};

extern GyroClass Gyro;

#endif

