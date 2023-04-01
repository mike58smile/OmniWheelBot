// Motors.h

#ifndef _MOTORS_h
#define _MOTORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class MotorsClass
{
 protected:

 public:
	 /**
	  * \brief Initialize motor pinouts and motor driver
	  * \note Use in void setup()
	  */
	 void init();

	 /**
	  * \brief Stop Motor (Puts HIGH on motor inputs)
	  */
	 void Stop();

	 /**
	  * \brief Set Speed of both motors
	  */
	 void Speed(int Spd1, int Spd2);
};

#endif

