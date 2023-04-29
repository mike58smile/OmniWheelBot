/*****************************************************************//**
 * \file   Drive.h
 * \brief  Drive class header
 * \details Implementing all movements -> it control Motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#ifndef _DRIVE_h
#define _DRIVE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "State.h"
#include <Encoder.h>
#include "Motors.h"

 /**
 * \brief Class implementing all movements, reading from Encoders
 */
class DriveClass
{
 private:
	 StateClass& State; ///< Storage for all shared variables 
	 MotorsClass Motors; ///< motory
	 static unsigned long currentTime; ///< Current time updated with micros() in void read()
	 static unsigned long previousTime; ///< Previous time updated with currentTime when currentTime >= TimerSpeedDelay_uS

	 /**
	  * \brief Read data from encoders - calculate speeds with period = TimerSpeedDelay_uS
	  * \note Time is measured using micros()
	  */
	 void read();
 public:
	 Encoder enc1; ///< motor 1
	 Encoder enc2; ///< motor 2
	 /**
	  * \brief C'tor from StateClass, also initialize Encoder objects
	  * \note This is the only constructor
	  * \param state Reference to storage for all shared variables 
	  */
	 explicit DriveClass(StateClass& state) : State(state), Motors(state), enc1(State.Enc1_1, State.Enc1_2), enc2(State.Enc2_1, State.Enc2_2) {}
	 
	 /**
	  * \brief Initialize motors
	  * \note Use in void setup()
	  */
	 void init();

	 /**
	  * \brief Drive loop
	  * \note Use in void loop()
	  */
	 void loop();

};

extern DriveClass Drive;

#endif
