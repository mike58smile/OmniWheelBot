/*****************************************************************//**
 * \file   Gyro.h
 * \brief  Gyro	class header
 * \details Class sensing and processing the data from accelerometer/gyro MPU6050
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#ifndef _GYRO_h
#define _GYRO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "State.h"
#include <Wire.h>
#include <MPU6050.h>
#include <MegunoLink.h>
#include "Filter.h" //

class GyroClass
{
 protected:
	 ExponentialFilter<long> ADCFilter; // Create a new exponential filter with a weight of 10 and initial value of 0. 
 public:
	 GyroClass() : ADCFilter(5, 0) {}
	 MPU6050 mpu;
	 void checkSettings();
	 void init();
	 float read(bool raw = 0, bool useFilter = 0);
};

extern GyroClass Gyro;

#endif

