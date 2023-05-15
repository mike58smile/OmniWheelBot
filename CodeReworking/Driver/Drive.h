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
#include <timer.h>
#include <Encoder.h>
#include <PID_v1.h>
#include "State.h"
#include "Motors.h"

constexpr auto SpeedRampDelayCalib = 80; ///< 1/Slope of speed accelerating ramp during calibration - i.e. equals to 1/acceleration (when smaller - it accelerates more quickly)

enum class CalibState { Init, Motor1_f, Motor1_b, Motor2_f, Motor2_b, End,   Size }; ///< CalibState enum, "Size" is a little trick - contains number of elements in this enum
#define calibStatePrint CalibStatePrint[static_cast<int>(calibState)] ///< Use for printing calibState enum
 /**
 * \brief Class implementing all movements, reading from Encoders
 */
class DriveClass final
{
 private:
	 CalibState calibState = CalibState::Init; ///< Enum for Deadband calibration state machine 
	 MotorsClass Motors{}; ///< Motors object of MotorsClass class implementing speed control of 2 motors
	 Timer timer; ///< Basic timer used for delayes when moving betweeen states
	 static unsigned long currentTime; ///< Current time updated with micros() in void read()
	 static unsigned long previousTime; ///< Previous time updated with currentTime when currentTime >= TimerSpeedDelay_uS

	 //Ramp init variables
	 bool rampInitDone[2] = { 0,0 }; ///< Indicates inicialization of ramp
	 Timer Timer_ramp1; ///< Not used - just for array of timers
	 Timer Timer_ramp2; ///< Not used - just for array of timers
	 Timer* Timer_ramp[2] = { &Timer_ramp1, &Timer_ramp2 };

	 bool accTillRotatingDone[2] = { 0, 0 };
	 bool deccTillRotatingDone[2] = { 0, 0 };
	 struct calibData {
		 bool endCalib = false;
		 int endSpeedPWM = 0;
		 float endRealSpeed = 0;
	 };

	 /**
	  * \brief Read data from encoders - calculate speeds with period = TimerSpeedDelay_uS
	  * \note Time is measured using micros()
	  */
	 void read();

	 double pid_Set1 = 0, pid_In1 = 0, pid_Out1 = 0; ///< Define signals for speed PID reg on motor 1
	 double pid_Set2 = 0, pid_In2 = 0, pid_Out2 = 0; ///< Define signals for speed PID reg on motor 2

	 //DeadbandCalib
	 unsigned long currentTime_C = 0, previousTime_C = 0; ///< Variables used for time reading in Deadband calibration
 public:
	 const char* CalibStatePrint[static_cast<int>(CalibState::Size)] = { "Init", "Motor1_f", "Motor1_b", "Motor2_f", "Motor2_b", "End" }; ///< Used for printing calibState enum
	 PID pid1; ///< PID object for motor 1
	 PID pid2; ///< PID object for motor 2
	 Encoder enc1; ///< Encoder for motor 1
	 Encoder enc2; ///< Encoder for motor 2

	 /**
	  * \brief C'tor from StateClass, also initialize Encoder and PID objects
	  * \note This is the only constructor
	  * \param state Reference to storage for all shared variables 
	  */
	 DriveClass() : enc1(State.Enc1_1, State.Enc1_2), enc2(State.Enc2_1, State.Enc2_2),
	 pid1(&pid_In1, &pid_Out1, &pid_Set1, State.Kp_1, State.Ki_1, State.Kd_1, DIRECT), pid2(&pid_In2, &pid_Out2, &pid_Set2, State.Kp_2, State.Ki_2, State.Kd_2, DIRECT){}
	 
	 /**
	  * \brief Initialize motors and sets operating mode and output limits of speed PID regulators
	  * \note Use in void setup()
	  */
	 void init();

	 /**
	  * \brief Drive loop
	  * \note Use in void loop() after Comm.loop()!
	  */
	 void loop();

	 void rampInit(bool motSelect, int TimeSlope, int SpeedBegin = 0);
	 void rampUpdate(bool motSelect, int increment);
	 void AccTillRotating_init(bool motSelect, int TimeSlope);
	 bool AccTillRotating_update(bool motSelect, int increment, unsigned int endSpeed = 0);
	 
	 void DeccTillRotating_init(bool motSelect, int timeSlope, int speedBegin = 0);
	 bool DeccTillRotating_update(bool motSelect, unsigned int decrement, unsigned int endSpeed = 0);

	 bool CalibDeadband();
};

extern DriveClass Drive;

#endif
