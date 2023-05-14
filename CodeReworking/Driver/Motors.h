/*****************************************************************//**
 * \file   Motors.h
 * \brief  Motors class header
 * \details Used for direct motor control
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#ifndef _MOTORS_h
#define _MOTORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "State.h"

/**
 * \brief Class directly controling motors
 */
class MotorsClass
{
 protected:
	 StateClass& State; ///< Storage for all shared variables 
 public:
	 
	 /**
	  * \brief C'tor from StateClass
	  * \param state Reference to storage for all shared variables
	  */
	 MotorsClass(StateClass& state) : State(state) {};

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
	  * \brief Set Speed of single motor
	  * \param motSelect Which motor to select 0/1
	  * \param Spd Speed in PWM
	  */
	 void SpeedSingle(bool motSelect, int Spd);

	 void skuska(int a);

	 /**
	  * \brief Set Speed of both motors
	  */
	 void Speed(int Spd1, int Spd2);

	 void Speed_1(int Spd);

	 void Speed_2(int Spd);


};

#endif

