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

/*************************************************** Setup variables *********************************************************************/
const unsigned long TimerSpeedDelay_mS = 30; ///< Speed reading from encoder time period - Change only this !!
/*****************************************************************************************************************************************/
const unsigned long TimerSpeedDelay_uS = TimerSpeedDelay_mS * 1000; ///< Period of reading speed (Period of TIMER_1 interrupts)
const float num = TWO_PI / (979.2 * (TimerSpeedDelay_mS / 1000.0));

inline float EncToRealSpd(int EncSpeed) {
	return(float(EncSpeed) * num);
}

static inline int8_t sign(int val) {
	if (val < 0) return -1;
	if (val == 0) return 0;
	return 1;
}
// User defined data types:

using Pin = const uint8_t;

#define printState_comm State.stateArr_comm[static_cast<int>(State.state_comm)] //inline function is much better
#define printState_actual State.stateArr_actual[static_cast<int>(State.state_actual)] //inline function is much better
#define printState_measType State.stateArr_measType[static_cast<int>(State.meas.state_measType)] //inline function is much better
	 
enum class State_measType { Calib, Ramp, Ramp_optim,   Size }; ///< Meassurement type state - have to be same as in Controller, Size contains number of elements in enum, used for printing
static const char* stateArr_measType[static_cast<int>(State_measType::Size)] = { "Calib", "Ramp", "Ramp_optim" }; ///< Array for printing meas.state_measType 

enum class State_actual { Setup, Speed, Stop, Size }; ///< Actual state in which the driver operates, Size contains number of elements in enum, used for printing, Size contains number of elements in enum, used for printing
static const char* stateArr_actual[static_cast<int>(State_actual::Size)] = { "Setup", "Speed", "Stop" }; ///< Array for printing state_actual

enum class State_comm { Stop, Wait, SpeedPWM, SpeedReal, ChangeConstPID, CalibDeadBand, Meas1, Unknown, Meas, Size }; ///< Communication state in which controller wants the driver to be in, only changed in Comm and SerialControl, Size contains number of elements in enum, used for printing
static const char* stateArr_comm[static_cast<int>(State_comm::Size)] = { "Stop", "Wait", "SpeedPWM", "SpeedReal", "ChangeConstPID", "CalibDeadBand", "Unknown" }; ///< Array for printing state_comm

/**
 * \brief Class for variables storage (only header, without methods) 
 */
class StateClass
{
 public:
	 State_actual state_actual = State_actual::Setup; ///< Actual state in which the driver operates, Size contains number of elements in enum, used for printing
	 State_comm state_comm = State_comm::Stop; ///< Communication state in which controller wants the driver to be in, only changed in Comm and SerialControl
	 State_comm state_commPrev = State_comm::Stop; ///< Previous communication state
	 
	 static const int address = 0x10; ///< Define I2C address of this Driver - !!Need to be changed for different driver

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

	 const int maxSpeed = 80; ///< Value of maximal speed in PWM allowed for MotorsClass

// Regulator parameters
	 double Kp_1 = 1, Ki_1 = 0, Kd_1 = 0; ///< Speed PID constants for motor 1
	 double Kp_2 = 0, Ki_2 = 0, Kd_2 = 0; ///< Speed PID constants for motor 2

	 int motor1DeadBandPWM[2] = { 0,0 }; ///< [forward,backward] - What is the minimum PWM value on which Motor 1 starts rotating
	 int motor2DeadBandPWM[2] = { 0,0 }; ///< [forward,backward] - What is the minimum PWM value on which Motor 2 starts rotating
	 int motorDeadBandPWM[2] = { 0, 0 };
	 int motor1DeadBandReal[2] = { 0,0 }; ///< [forward,backward] - What is the minimum speed [rad/s] on which Motor 1 starts rotating
	 int motor2DeadBandReal[2] = { 0,0 }; ///< [forward,backward] - What is the minimum speed [rad/s] on which Motor 2 starts rotating
	 //float motorDeadBandReal[2] = { 0, 0 };
	 bool CalibEnd = 0;  ///< Bool signaling End of calibration

// Non const variables - will be changed during program

	 int actualSpeed[2] = { 0,0 }; ///< Actual speed in PWM of two motors which is sent by analogWrite, (0 - 255)
	 int requiredSpeed[2] = { 0,0 }; ///< Required speed of two motors in PWM by Controler (recieved through I2C), write only in Comm class! (0 - 255)
	 int actualEncSpeed[2] = { 0,0 }; ///< Actual number of encoder pulses updated every period of reading speed
	 int requiredEncSpeed[2] = { 0,0 };
	 float actualRealSpeed[2] = { 0,0 }; ///< Actual real speed of two motors in rad/s, write only in TimerSpeedHandler interrupt routine
	 float requiredRealSpeed[2] = { 0,0 }; ///< Required real speed of two motors in rad/s by Controler (recieved through I2C), write only in Comm class!

// Measurements variables:
	 struct Meas {
		 State_measType state_measType;
		 unsigned int motSelect = 0;
		 int temp = 0;
	 } meas;

	 unsigned long readSpeed = 0; ///< Speed of reading from encoders [ms]
	 int tempMode;
};

extern StateClass State;

#endif


/** Konzultacia
 * Drive - explicit konstruktor
 * Pin - aky DT?
 * constexpr, define, static inline - pri Comm
 * State ako extern ale zaroven vstupuje do konstruktorov s rovnakym nazvom
 * Je casovo narocne pristupovat do State? Konstanty regulatora maju byt v State alebo ako prvky danej triedy? (verejne)
 * 
 * 
 */


/* Garbage:
* 
* //#define USE_TIMER_2 true //Use before TimerInterrupt library included - defines which timer to use for interrupts (but it isn't working with Encoders)
* 
* inline static const char* printState_measType() {
	return stateArr_measType[static_cast<int>(State.meas.state_measType)];
}
inline static const char* printState_actual() {
 return stateArr_actual[static_cast<int>(State.state_actual)];
}
inline static const char* printState_comm() {
 return stateArr_comm[static_cast<int>(State.State.state_comm)];
}
* 
Initialization of static const array in .h file
	 static const uint8_t Enc1Pin(size_t index) {
		 static const uint8_t x[] = { 1,2 };
		 return x[index];
	 }
*/

/* RPM Debounce
 Relative to RPM readout and triggering to start  a thread path:

  Index debounce will affect the sensitivity of the index signal. IT basically is how many interrupt periods the signal must be present, or not present before a change is actually sensed in that line. SO if set to 2 for example, when the index appears it will be ignored for 2 periods to make sure it isnt noise. Same with when it disappears. Setting debounce too high will make the index go away altogether.

  Since the length of the index is dependent on spindle speed, minimum length is variable, but the time must be at least 1 period at a debounce of zero. SO in 25000, thats 40us.
  If a person notices a dropoff in RPM at a certain speed, they need usually a lower index debounce. Index inputs from an encoder are usually pretty short and will limit speed readings at some point as you go higher.

If you have the debounce setting too high the axis will just sit there waiting to confirm the rpm
before the start of a thread path.

Don't have specifics relative to switches .
 */

//READ spd with millis
//     unsigned long currentMillis_1 = 0;
//	   unsigned long previousMillis_1 = 0;
// 
 //    currentMillis_1 = millis();
 //    if ((currentMillis_1 - previousMillis_1) >= 200) {
 //        encSpeed[0] = abs(Drive.enc1.read());
 //        Drive.enc1.write(0);
 //        encSpeed[1] = abs(Drive.enc2.read());
 //        Drive.enc2.write(0);
 //;       previousMillis_1 = currentMillis_1;
 //    }


// Using timer interrupt
// INCLUDES
// #include <ISR_Timer.h>
//#include <TimerInterrupt.hpp>
//#include <TimerInterrupt.h>
//#include <ISR_Timer.hpp> 

//SETUP:
// Timer init
//ITimer2.init();
//if (ITimer2.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler))
//{
//    Serial.println(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
//}
//else
//    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

//GLOBAL in .ino
//Global variables
//static volatile bool flagReadSpeed = 0; ///< Will be set true in interrupt routine
//static volatile int interruptNum = 0; ///< Will increment in interrupt routine
//void TimerSpeedHandler()
//{
//    flagReadSpeed = 1;
//    ++interruptNum;
//}

//In void loop:
//if (flagReadSpeed) {
//    Drive.read();
//    flagReadSpeed = 0;
//    interruptNum = 0;
//}

//In Drive.read:
//State.encSpeed[0] = abs(enc1.read());
//enc1.write(0);
//State.encSpeed[1] = abs(enc2.read());
//enc2.write(0);

//In Comm::RequestEvent:
//char strBuffer[7];
//Wire.write(dtostrf(State.actualRealSpeed[0], 7, 2, strBuffer)); //!odskusat ci funguje
//Wire.write(dtostrf(State.actualRealSpeed[1], 7, 2, strBuffer));//!odskusat ci funguje  

//RAMP:
//bool ramp(int motor, int beginPWM = 0, float slope = 1);

//Motors Speed:
//// Set speed of first motor
//if (Spd1 <= 0) {
//    analogWrite(State.M1_LPWM, 0);
//    analogWrite(State.M1_RPWM, abs(Spd1));
//}
//else{
//    analogWrite(State.M1_RPWM, 0);
//    analogWrite(State.M1_LPWM, abs(Spd1));
//}

//// Set speed of second motor
//if (Spd2 <= 0) {
//    analogWrite(State.M2_LPWM, 0);
//    analogWrite(State.M2_RPWM, abs(Spd2));
//}
//else {
//    analogWrite(State.M2_RPWM, 0);
//    analogWrite(State.M2_LPWM, abs(Spd2));
//}
//
//// Save current motor speeds to State
//State.actualSpeed[0] = Spd1;
//State.actualSpeed[1] = Spd2;
