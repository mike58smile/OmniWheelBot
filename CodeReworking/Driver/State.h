// State.h

#ifndef _STATE_h
#define _STATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// User defined data types:
enum MainState { Reset, Speed }; //enum class is better cause the name of the enum elements can be used outside the enum as variables
using Pin = const uint8_t;

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
	 //define main state in which the driver operates -> Mainly controller changes this state
	 MainState mainState = Reset;
	 
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