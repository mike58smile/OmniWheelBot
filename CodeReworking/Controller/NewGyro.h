// NewGyro.h

#ifndef _NEWGYRO_h
#define _NEWGYRO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class NewGyroClass
{
 protected:
	 Adafruit_MPU6050 mpu;
	 float offset = 0.0;
 public:
	void init();
	float read();
};

#endif