/*****************************************************************//**
 * \file   State.h
 * \brief  State class header
 * \details Storage for all variables shared between classes
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#ifndef _STATE_h
#define _STATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

using Pin = const uint8_t;

 /**
  * \brief Class for variables storage (only header, without methods)
  */
class StateClass
{
 protected:

 public:
	 int actualSpeed[2] = { 0,0 }; ///< Actual speed in PWM of two motors which is sent by analogWrite in Driver in range (0 - 255)
	 float actualRealSpeed[2] = { 0,0 }; ///< Actual real speed of two motors in rad/s
	 int adress[2] = { 0x10, 0x11 }; ///< Adress of drivers - 0x10 = motors 1,2 ; 0x11 = motors 3,4

	 static Pin IRPin = 40; ///< Define IR signal recieving pin
};

extern StateClass State;

#endif

