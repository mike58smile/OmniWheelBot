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
//#define ENCODER_OPTIMIZE_INTERRUPTS //Encoder reading optimization - written in assembly - but not working - with first came interrupt the program freezes
#include <Encoder.h>
#include <PID_v1.h>
#include "State.h"
#include "Motors.h"

 /**
 * \brief Class implementing all movements, meassurements, reading from Encoders
 */
class DriveClass final
{
 private:
	 //-----------------------------Setup variables-----------------------------
	 const int SpeedRampDelayCalib = 80; ///< 1/Slope of speed accelerating ramp during calibration - i.e. equals to 1/acceleration (when smaller - it accelerates more quickly)
	struct OptimizedSpd {
		unsigned int zeroPWM = 3;
		unsigned int minPWM = 20;
	} optimizedSpd;
	 //-------------------------------------------------------------------------

#pragma region Variables_Speed 
	 //-----------------------------Reading/Controlling speed variables/functions/objects-----------------------------
	 MotorsClass Motors{}; ///< Motors object of MotorsClass class implementing speed control of 2 motors
	 Encoder enc1; ///< Encoder for motor 1
	 Encoder enc2; ///< Encoder for motor 2
	 unsigned long currentTime = 0; ///< Current time updated with micros() in void read()
	 unsigned long previousTime = 0; ///< Previous time updated with currentTime when currentTime >= TimerSpeedDelay_uS
	 
	 /**
	  * \brief Read data from encoders - calculate speeds with period = TimerSpeedDelay_uS
	  * \note Time is measured using micros()
	  */
	 void read();
	 //-------------------------------------------------------------------------------------------------------
#pragma endregion - Reading/Controlling speed variables/functions/objects

#pragma region Variables_Pid
public:
	 double pid_Set1 = 0, pid_In1 = 0, pid_Out1 = 0; ///< Define signals for speed PID reg on motor 1
	 double pid_Set2 = 0, pid_In2 = 0, pid_Out2 = 0; ///< Define signals for speed PID reg on motor 2
	 PID pid1; ///< PID object for motor 1
	 PID pid2; ///< PID object for motor 2
#pragma endregion - PID variables(signals) and objects

	 
#pragma region Variables_Ramp
private:
	 //-----------------------------Ramp function variables/functions/objects-----------------------------
	 bool rampInitDone[2] = { 0,0 }; ///< Indicates inicialization of ramp
	 Timer Timer_ramp1; ///< Timer for first motor ramp - explicitly used just in array of timers
	 Timer Timer_ramp2; ///< Timer for second motor ramp - explicitly used just array of timers
	 Timer* Timer_ramp[2] = { &Timer_ramp1, &Timer_ramp2 }; ///< Array of ramp timers for both motors - used in ramp functions
public:
	/**
	 * \brief Initialize ramp timer and sets rampInitDone to true
	 * \param motSelect {0,1} motor
	 * \param timeSlope 1/Slope of speed accelerating ramp
	 */
	 void rampInit(bool motSelect, int timeSlope);

	 /**
	  * \brief Initialize ramp timer and sets rampInitDone to true
	  * \param motSelect {0,1} motor
	  * \param timeSlope 1/Slope of speed accelerating ramp
	  * \param SpeedBegin Beginning speed of ramp - also sent to motors (when optim = 1 -> add optimizedSpd.minPWM for beginning speed)
	  * \param optim Use optimized PWM speed
	  * \param TimeSlope 1/Slope of speed accelerating ramp
	  */
	 void rampInit(bool motSelect, int timeSlope, int SpeedBegin, bool optim = 0);

	 /**
	  * \brief Increment speed when timer >= timeSlope (defined when called rampInit())
	  * \param motSelect {0,1} motor
	  * \param increment Value added to motor speed in each period [PWM]
	  * \param optim Use optimized PWM speed
	  * \warning rampInit has to be done and all functions which uses rampUpdate have to set rampInit to false after end of procedure
	  */
	 void rampUpdate(bool motSelect, int increment, bool optim = false);
	 //---------------------------------------------------------------------------------------------------
#pragma endregion - Variables/functions/objects

 public:
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
	  * \brief Reads from encoders and handles main state machine - State_comm
	  * \note Use in void loop() after Comm.loop() and SerialControl.loop()!
	  */
	 void loop();

private:
	 /**
	  * \brief Optimize PWM controlling motors - make the system more linear
	  * \param PWMspeed Input raw speed [PWM]
	  * \param zeroVal Absolut speed range boundary where zero is produced on output [PWM]
	  * \param minPWM Minimal absolute speed produced on output [PWM]
	  * \warning zeroVal < minPWM AND minPWM
	  * \return Optimized speed [PWM]
	  */
	 inline int PWMtoOptimizedPWM(int PWMspeed, unsigned int zeroVal, unsigned int minPWM);
	 
	 /**
	  * \brief Optimize PWM controlling motors - make the system more linear (calls main function using member variables of class) - chained overload
	  * \param PWMspeed Input raw speed [PWM]
	  * \return Optimized speed [PWM]
	  */
	 inline int PWMtoOptimizedPWM(int PWMspeed) {
		 return PWMtoOptimizedPWM(PWMspeed, optimizedSpd.zeroPWM, optimizedSpd.minPWM);
	 }


#pragma region Functions_Accelerations 
public:
	/**
	 * \brief Init acceleration till motor starts rotating - calls rampInit and sets accTillRotatingDone = 0
	 * \param motSelect {0,1} motor
	 * \param timeSlope 1/Slope of speed accelerating ramp
	 */
	 void AccTillRotating_init(bool motSelect, int timeSlope);

	 /**
	  * \brief Accelerate till motor starts rotating at speed > endSpeed
	  * \param motSelect {0,1} motor
	  * \param increment Value added to motor speed in each period [PWM]
	  * \param endSpeed Function terminating speed [enc]
	  * \return true if current speed > endSpeed, else false
	  */
	 bool AccTillRotating_update(bool motSelect, int increment, unsigned int endSpeed = 0);
	 
	 /**
	  * \brief Init decceleration till motor stops rotating
	 *	\param motSelect {0,1} motor
	 *	\param timeSlope 1/Slope of speed deccelerating ramp
	  * \param speedBegin Beginning speed of deccelerating, also sent to motors
	  */
	 void DeccTillRotating_init(bool motSelect, int timeSlope, int speedBegin = 0);

	 /**
	  * \brief Deccelerate till motor stops rotating or ends at endSpeed
	  * \param motSelect {0,1} motor
	  * \param decrement Value subtracted from motor speed in each period [PWM]
	  * \param endSpeed Function terminating speed [enc]
	  * \return true if current speed <= endSpeed
	  */
	 bool DeccTillRotating_update(bool motSelect, unsigned int decrement, unsigned int endSpeed = 0);

	 /**
	  * \brief Init acceleration till PWM speed
	  * \param motSelect {0,1} motor
	  * \param timeSlope 1/Slope of speed accelerating ramp
	  */
	 void AccTillPWM_init(bool motSelect, int timeSlope);

	 /**
	  * \brief Init acceleration from speedBegin to PWM speed
	  * \param motSelect {0,1} motor
	  * \param timeSlope 1/Slope of speed accelerating ramp
	  * \param speedBegin Beginning speed of accelerating, also sent to motors
	  * \param optim Use optimized PWM speed
	  */
	 void AccTillPWM_init(bool motSelect, int timeSlope, int speedBegin, bool optim = 0);

	 /**
	  * \brief Accelerate till motor starts rotating at speed >= endSpeed
	  * \param motSelect {0,1} motor
	  * \param increment Value added to motor speed in each period [PWM]
	  * \param endSpeed Function terminating speed [PWM]
	  * \param optim Use optimized PWM speed
	  * \return true if current speed >= endSpeed (when optim = 1 -> true if current speed >= endSpeed + optimizedSpd.minPWM)
	  * \note Maybe the return calc when optim = 1 is not fully correct
	  */
	 bool AccTillPWM_update(bool motSelect, int increment, int endSpeed, bool optim = 0);

	 /**
	  * \brief Init decceleration till PWM speed
	  * \param motSelect {0,1} motor
	  * \param timeSlope 1/Slope of speed accelerating ramp
	  */
	 void DeccTillPWM_init(bool motSelect, int timeSlope);

	 /**
	  * \brief Init decceleration from speedBegin to PWM speed
	  * \param motSelect {0,1} motor
	  * \param timeSlope 1/Slope of speed accelerating ramp
	  * \param speedBegin Beginning speed of deccelerating, also sent to motors
	  * \param optim Use optimized PWM speed
	  */
	 void DeccTillPWM_init(bool motSelect, int timeSlope, int speedBegin, bool optim = 0);

	 /**
	  * \brief Deccelerate till motor starts rotating at speed > endSpeed
	  * \param motSelect {0,1} motor
	  * \param decrement Value subtracted from motor speed in each period [PWM]
	  * \param endSpeed Function terminating speed [PWM]
	  * \param optim Use optimized PWM speed
	  * \return true if current speed <= endSpeed (when optim = 1 -> true if current speed >= endSpeed - optimizedSpd.minPWM)
	  * \note Maybe the return calc when optim = 1 is not fully correct
	  */
	 bool DeccTillPWM_update(bool motSelect, unsigned int decrement, int endSpeed, bool optim = 0);

private:
	bool accTillRotatingDone[2] = { 0, 0 }; ///< Indication of acceleration done - used in AccTillRotating...() functions
	bool deccTillRotatingDone[2] = { 0, 0 }; ///< Indication of decceleration done - used in DeccTillRotating...() functions
	bool accTillPWMDone[2] = { 0,0 }; ///< Indication of acceleration done - used in AccTillPWM...() functions
	bool deccTillPWMDone[2] = { 0,0 }; ///< Indication of acceleration done - used in DeccTillPWM...() functions
#pragma endregion

#pragma region Functions_Meassurements
public:
	/**
	 * \brief Deadband meassurement state machine
	 * \return true after end of calibration - state enum is in its last state (End)
	 */
	 bool Meassure_deadband(); 

	/**
	 * \brief Linearity ramp meassurement state machine
	 * \param maxSpeed Maximal achieved speed (for optim = 1 -> add optimizedSpd.minPWM for maximal achieved speed)
	 * \param motSelect {0,1} motor
	 * \param optim Use optimized PWM speed
	 * \return true after end of meassurement - state enum is in its last state (End)
	 */
	 bool Meassure_linearityRamp(unsigned int maxSpeed = 70, int motSelect = 0, bool optim = 0);
#pragma endregion

#pragma region State_Enums
private:
	enum class State_deadbandMeas { Init, Motor1_f, Motor1_b, Motor2_f, Motor2_b, End,   Size } state_deadbandMeas = State_deadbandMeas::Init; ///< Enum for Deadband calibration state machine, Size contains number of elements in enum, used for printing
	const char* printState_deadbandMeas[static_cast<int>(State_deadbandMeas::Size)] = { "Init", "Motor1_f", "Motor1_b", "Motor2_f", "Motor2_b", "End" }; ///< Array for printing state_deadbandMeas 
	
	enum class State_rampMeas { InitDecc, Decc, InitAcc, Acc, End, Size } state_rampMeas = State_rampMeas::InitAcc; ///< Enum for ramp deadband (linearity) meassurements, used in Meassure_linearityRamp(), Size contains number of elements in enum, used for printing
	const char* printState_rampMeas[static_cast<int>(State_rampMeas::Size)] = { "InitDecc", "Decc", "InitAcc", "Acc", "End" }; ///< Array for printing state_rampMeas
public:
	inline const char* printState_deadband() {
		return printState_deadbandMeas[static_cast<int>(state_deadbandMeas)];
	}
	inline const char* printState_ramp() {
		return printState_deadbandMeas[static_cast<int>(state_deadbandMeas)];
	}
#pragma endregion State_Enums - for state machines, also printing functions

};

extern DriveClass Drive;

#endif

//Garbage:
///**
// * \brief Struct for Deadband calibration out - not used
// */
//struct calibData {
// bool endCalib = false;
// int endSpeedPWM = 0;
// float endRealSpeed = 0;
//};

//#define calibStatePrint CalibStatePrint[static_cast<int>(calibState)] ///< Use for printing state_deadbandMeas enum - needs to make inline function instead!!