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

#include <Encoder.h>
#include <PID_v1.h>
#include "State.h"
#include "Motors.h"

constexpr auto SpeedRampDelayCalib = 80000; ///< Need to change
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

	 double pid_Set1 = 0, pid_In1 = 0, pid_Out1 = 0;
	 double pid_Set2 = 0, pid_In2 = 0, pid_Out2 = 0;

	 //DeadbandCalib
	 unsigned long currentTime_C = 0, previousTime_C = 0;
 public:

	 PID pid1; ///< PID object for motor 1
	 PID pid2; ///< PID object for motor 2
	 Encoder enc1; ///< Encoder for motor 1
	 Encoder enc2; ///< Encoder for motor 2

	 /**
	  * \brief C'tor from StateClass, also initialize Encoder objects
	  * \note This is the only constructor
	  * \param state Reference to storage for all shared variables 
	  */
	 explicit DriveClass(StateClass& state) : State(state), Motors(state), enc1(State.Enc1_1, State.Enc1_2), enc2(State.Enc2_1, State.Enc2_2),
	 pid1(&pid_In1, &pid_Out1, &pid_Set1, State.Kp_1, State.Ki_1, State.Kd_1, DIRECT), pid2(&pid_In2, &pid_Out2, &pid_Set2, State.Kp_2, State.Ki_2, State.Kd_2, DIRECT){}
	 
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
