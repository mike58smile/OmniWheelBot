/*****************************************************************//**
 * \file   Gyro.h
 * \brief  Gyro	class header
 * \details For sensing and processing the data from accelerometer/gyro MPU6050
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
#include <MPU6050_tockn.h>

class GyroClass
{
 protected:
	 MPU6050 mpu6050;
	 StateClass& State;
 public:
	 /**
	  * \brief C'tor from StateClass, also initialize mpu6050 object
	  * \note This is the only constructor
	  * \param state Reference to storage for all shared variables
	  */
	 GyroClass(StateClass& state) : State(state), mpu6050(Wire) {}


	 void init();
};

extern GyroClass Gyro;

#endif

