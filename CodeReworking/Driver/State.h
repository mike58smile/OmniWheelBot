/*****************************************************************//**
 * \file   State.h
 * \brief  State class 
 * \details Storage for all variables shared between classes
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#ifndef _STATE_h
#define _STATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// User defined data types:
enum MainState { Setup, Speed, Stop }; //enum class is better cause the name of the enum elements can be used outside the enum as variables
enum CommState { Stop, Wait, Speed};
using Pin = const uint8_t;

/**
 * \brief Class for variables storage (only header, without methods) 
 */
class StateClass
{
 protected:

 public:
// Define pins
	 //Motor1
	 static Pin M1_LPWM = 5;
	 static Pin M1_RPWM = 6;
	 static Pin M1_REN = 4;
	 static Pin M1_LEN = 7;
	 //Motor2
	 static Pin M2_RPWM = 10;
	 static Pin M2_LPWM = 11;
	 static Pin M2_REN = 8;
	 static Pin M2_LEN = 9;
	 //define Encoder pins
	 static Pin Enc1_1 = 2;
	 static Pin Enc1_2 = 12;
	 static Pin Enc2_1 = 3;
	 static Pin Enc2_2 = A0;

// Other definitions
	 static const int address = 0x10;
// Other stuff
	 //define actual state in which the driver operates 
	 MainState actualState = Setup;

	 //define communication state in which controller wants the driver to be in, only changed in Comm
	 CommState commState = Wait;

	 int actualSpeed[2] = { 0,0 }; ///< Actual speed of two motors which is sent by analogWrite
	 int requiredSpeed[2] = { 0,0 }; ///< Required speed of two motors by Controler (recieved through I2C), write only in Comm class!
};

extern StateClass State;

#endif

/* Garbage:
Initialization of static const array in .h file
	 static const uint8_t Enc1Pin(size_t index) {
		 static const uint8_t x[] = { 1,2 };
		 return x[index];
	 }
*/