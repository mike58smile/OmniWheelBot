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
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Encoder.h>
#include "State.h"
#include "Motors.h"

 /**
 * \brief Class implementing all movements, reading from Encoders
 */
class DriveClass
{
 protected:
	 StateClass& State;
	 MotorsClass Motors; ///< motory
	 //friend void TimerSpeedHandler(); ///< Function to handle TIMER_1 interrupt routine for reading speed
 public:
	 Encoder enc1; ///< motor 1
	 Encoder enc2; ///< motor 2
	 /**
	  * \brief C'tor from StateClass, also initialize Encoder objects
	  * \note This is the only constructor
	  * \param state
	  */
	 //explicit DriveClass() : enc1(State.Enc1_1, State.Enc1_2), enc2(State.Enc2_1, State.Enc2_2) {}
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

//extern DriveClass Drive;

#endif

