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

constexpr auto BaudRate = 115200; ///< Baudrate speed

/**
 * \brief Read 16bit int from master I2C
 * \return int
 */
inline int WireReadI() {
	return Wire.read() | Wire.read() << 8;
}

/**
 * \brief Write 16bit int to master I2C
 * \param int
 */
inline void WireWriteI(int iValue) {
	Wire.write(iValue);
	Wire.write((iValue >> 8));
}

/**
 * \brief For float to Byte conversion
 * \note Union stores value on the same memory place - we wrote to floatValue and read from buffer which points to same memmorhy location as float so we read float value but outside it is considered char (buffer)
 * \warning If different architecture - char may be different from float
 * \note Get pointer to memmory where the float is located and converse it to char
 */
inline union BytesFloatConv {
	char buffer[4]; //size is 4 bytes
	float floatValue; //size is also 4 bytes
} bytesFloatConv;

/**
 * \brief Read 16bit int from master I2C
 * \return int
 */
inline float WireReadF() {
	for (int i = 0; i < 4; ++i)
		bytesFloatConv.buffer[i] = Wire.read();
	return bytesFloatConv.floatValue;
}

/**
 * \brief Write 32bit float to master I2C
 * \param int
 */
inline void WireWriteF(float fValue) {
	bytesFloatConv.floatValue = fValue;
	Wire.write(bytesFloatConv.buffer, 4);
}

/**
 * \brief Class for I2C and Serial communication
 */
class CommClass final
{
 public:

	 /**
	  * \brief Initialize Serial and I2C communication
	  * \note Use in void setup()
	  */
	 void init();
};

extern CommClass Comm;

#endif