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
inline int WireRead() {
	Wire.read() | Wire.read() << 8;
}

inline void WireWriteI(int iValue) {
	Wire.write(iValue);
	Wire.write((iValue >> 8));
}
//#define WireRead (Wire.read() | Wire.read() << 8)

#define WireWrite(int16){ \
Wire.write(int16); \
Wire.write((int16 >> 8)); \
}

class CommClass
{
 protected:
	 StateClass& State;
 public:
	 CommClass(StateClass& state) : State(state) {}
	void init();
	void loop();
};

extern CommClass Comm;

#endif

