/*****************************************************************//**
 * \file   Comm.h
 * \brief  Comm class header
 * \details Used for I2C communication with the Driver
 * 
 * \author xmisko06
 * \date   May 2023
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

constexpr auto BaudRate = 115200;
inline size_t countBytes(size_t numInt16, size_t numFloat32) {
	return numInt16 * 2 + numFloat32 * 4;
}
inline int WireReadI() {
	return (Wire.read() | Wire.read() << 8);
}
/**
 * \brief Byte to float conversion
 * \note Union stores value on the same memory place - we wrote to floatValue and read from buffer which points to same memmorhy location as float so we read float value but outside it is considered char (buffer)
 */
inline union BytesFloatConv {
	char buffer[4]; //size is 4 bytes
	float floatValue; //size is also 4 bytes
} bytesFloatConv;

inline float WireReadF() {
	for(int i = 0; i < 4; ++i)
		bytesFloatConv.buffer[i] = Wire.read();
	return bytesFloatConv.floatValue;
}

inline void WireWriteF(float fValue) {
	bytesFloatConv.floatValue = fValue;
	Wire.write(bytesFloatConv.buffer, 4);
}

inline void WireWriteI(int iValue) {
	Wire.write(iValue);
	Wire.write((iValue >> 8));
}
//#define WireReadI (Wire.read() | Wire.read() << 8)

//#define WireWrite(int16){ \
//Wire.write(int16); \
//Wire.write((int16 >> 8)); \
//}

class CommClass
{
 protected:
	 StateClass& State;

	 //For Serial Debug
	 char EnableSerialMode = 'x';
	 String SerialString = "";
	 int SerialInt = 0;
 public:
	 CommClass(StateClass& state) : State(state) {}
	 void init();
	 void loop();
	 void stop();
	 void SetPWM(int spd1, int spd2, int spd3, int spd4);
	 void SerialDebug();
};

extern CommClass Comm;

#endif

