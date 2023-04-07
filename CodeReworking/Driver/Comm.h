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
#define WireRead (Wire.read() | Wire.read() << 8)

#define WireWrite(int16){ \
Wire.write(int16); \
Wire.write((int16 >> 8)); \
}

/**
 * \brief Class for I2C and Serial communication
 */
class CommClass
{
 protected:
	 StateClass& State;

	 /**
	  * \brief I2C send data - Wire request event
	  */
	// friend void requestEvent();

	 /**
	  * \brief I2C get data - Wire recieve event
	  */
	 //friend void receiveData(int x);

 public:
	 CommClass(StateClass& state) : State(state){}
	 //CommClass(){}
	 /**
	  * \brief Initialize Serial and I2C communication
	  * \note Use in void setup()
	  */
	 void init();

};

extern CommClass Comm;

#endif

