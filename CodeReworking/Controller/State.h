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

//---Here are changable constants
constexpr int TimerSpeedDelay_mS = 20; ///< NEED TO BE SAME AS IN DRIVER!! Speed reading from encoder time period - Change only this !! [ms]
//-------------------------------
constexpr int TimerSpeedDelay_uS = TimerSpeedDelay_mS * 1000; ///< Period of reading speed (Period of TIMER_1 interrupts) [us]
constexpr float TimerSpeedDelay_S = TimerSpeedDelay_mS / 1000.0; ///< Period of reading speed (Period of TIMER_1 interrupts) [s]
const float speedConvSlope = TWO_PI / (979.2 * TimerSpeedDelay_S); ///< Speed conversion slope (Enc speed - Real speed

/**
 * \brief Convert [ENC] to [rad/s]
 * \param RealSpeed angular spd [rad/s]
 * \return [rad/s]
 */
inline float EncToRealSpd(int EncSpeed) {
	return(float(EncSpeed) * speedConvSlope);
}

/**
 * \brief Convert [rad/s] to [ENC]
 * \param RealSpeed angular spd [rad/s]
 * \return [rad/s]
 */
inline int RealToEncSpd(float RealSpeed) {
	return int(RealSpeed / speedConvSlope);
}

/**
 * \brief Sign function
 * \param val input value
 * \return -1 if negative, 0 if equal, else 1
 */
static inline int8_t sign(int val) {
	if (val < 0) return -1;
	if (val == 0) return 0;
	return 1;
}

/**
 * \brief Sign function
 * \param val input value
 * \return -1 if negative, 0 if equal, else 1
 */
static inline int signF(float val) {
	if (val < 0) return -1;
	if (val == 0) return 0;
	return 1;
}
using Pin = const uint8_t;


enum class ControlState { Stop, Wait, SpeedPWM, SpeedReal, SetPID, SetMeas, Unknown,   Size }; ///< Define CommState enum
#define controlStatePrint State.ControlStatePrint[static_cast<int>(State.controlState)] //inline function is much better
enum class State_movement{ IR_movement, CalcSpd, Circle, MeasGyro, PidGyro, Meas, ShowTime_Dir, ShowTime_Circ, MeasTypes}; ///< State of required movement
 /**
  * \brief Class for variables storage (only header, without methods)
  */
class StateClass
{
 protected:

 public:
	 int gyroUpdateMS = 100; ///< Period of reading from gyro
	 float Kp = 1.1, Ki = 2, Kd = 0; ///< PID gyro constants Kp = 1.5, Ki = 1.1, Kd = 0;

	 static Pin IRPin = 40; ///< Define IR signal recieving pin
	 int adress[2] = { 0x10, 0x11 }; ///< Adress of drivers - 0x10 = motors 1,2 ; 0x11 = motors 3,4
	 const char* ControlStatePrint[static_cast<int>(ControlState::Size)] = { "Stop", "Wait", "SpeedPWM", "SpeedReal", "SetPID", "Unknown" }; ///< Used for printing controlState enum
	 ControlState controlState = ControlState::Stop; ///< Define control state in which controller wants the driver to be in, only changed in Comm and SerialControl
	 State_movement state_movement = State_movement::ShowTime_Dir;
	 float wantedW = 0; ///< Wanted angular spd [rad/s]
	 float wantedV = 0; ///< Wanted linear spd [m/s]
	 float wantedRadius = 0.2; ///< Wanted circle trajectory diameter [m]
	 int wantedAlfa = 0; ///< Wanted Azimut (heading direction) [°] 
	 bool GyroRegON = 1, stopCirc = 1;

	 float Kp_1 = 0.8, Ki_1 = 8, Kd_1 = 0; ///< Speed PID constants for motor 1 , Ki = 8 nice
	 int actualSpeed[4] = { 0,0,0,0 }; ///< Actual speed in PWM of two motors which is sent by analogWrite in Driver in range (0 - 255)
	 int actualEncSpeed[4] = { 0,0,0,0 }; ///< Actual number of encoder pulses updated every period of reading speed
	 int requiredPWMSpeed[4] = { 0,0,0,0 }; ///< Required speed in driver [PWM]
	 int requiredEncSpeed[4] = { 0,0,0,0 }; ///< Required speed sent to Driver [enc]
	 float requiredSpeed[4] = { 0,0,0,0 }; ///< Required speed sent to Driver [rad/s]
	 float actualRealSpeed[4] = { 0,0,0,0 }; ///< Actual real speed of two motors in rad/s

	 int motSelectMeas = 0; ///< For meassuring purposes
};

extern StateClass State;

#endif

