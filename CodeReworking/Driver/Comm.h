/*****************************************************************//**
 * \file   Comm.h
 * \brief  Comm class header
 * \details Used for I2C communication with the Controller
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#ifndef _COMM_h
#define _COMM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "State.h"
#include <Wire.h>
#include <i2cdetect.h>

constexpr auto BaudRate = 115200;
inline int WireReadI() {
	return Wire.read() | Wire.read() << 8;
}
inline void WireWriteI(int iValue) {
	Wire.write(iValue);
	Wire.write((iValue >> 8));
}

/**
 * \brief For float to Byte conversion
 * \note Union stores value on the same memory place - we wrote to floatValue and read from buffer which points to same memmorhy location as float so we read float value but outside it is considered char (buffer)
 */
inline union BytesFloatConv {
	char buffer[4]; //size is 4 bytes
	float floatValue; //size is also 4 bytes
} bytesFloatConv;

inline float WireReadF() {
	for (int i = 0; i < 4; ++i)
		bytesFloatConv.buffer[i] = Wire.read();
	return bytesFloatConv.floatValue;
}

inline void WireWriteF(float fValue) {
	bytesFloatConv.floatValue = fValue;
	Wire.write(bytesFloatConv.buffer, 4);
}

/**
 * \brief Class for I2C and Serial communication
 */
class CommClass
{
 protected:
	 StateClass& State;
 public:
	 /**
	  * \brief C'tor from StateClass
	  * \param state Reference to storage for all shared variables
	  */
	 CommClass(StateClass& state) : State(state){}

	 /**
	  * \brief Initialize Serial and I2C communication
	  * \note Use in void setup()
	  */
	 void init();
};

extern CommClass Comm;

#endif