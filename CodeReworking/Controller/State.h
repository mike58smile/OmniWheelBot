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

static inline int8_t sign(int val) {
	if (val < 0) return -1;
	if (val == 0) return 0;
	return 1;
}
using Pin = const uint8_t;


enum class ControlState { Stop, Wait, SpeedPWM, SpeedReal, SetPID, Unknown,   Size }; ///< Define CommState enum
#define controlStatePrint State.ControlStatePrint[static_cast<int>(State.controlState)] //inline function is much better

 /**
  * \brief Class for variables storage (only header, without methods)
  */
class StateClass
{
 protected:

 public:
	 const char* ControlStatePrint[static_cast<int>(ControlState::Size)] = { "Stop", "Wait", "SpeedPWM", "SpeedReal", "SetPID", "Unknown" }; ///< Used for printing controlState enum
	 ControlState controlState = ControlState::Stop; ///< Define control state in which controller wants the driver to be in, only changed in Comm and SerialControl
	 static Pin IRPin = 40; ///< Define IR signal recieving pin

	 int actualSpeed[4] = { 0,0,0,0 }; ///< Actual speed in PWM of two motors which is sent by analogWrite in Driver in range (0 - 255)
	 float actualRealSpeed[4] = { 0,0,0,0 }; ///< Actual real speed of two motors in rad/s
	 int adress[2] = { 0x10, 0x11 }; ///< Adress of drivers - 0x10 = motors 1,2 ; 0x11 = motors 3,4
};

extern StateClass State;

#endif

