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

#define USE_TIMER_2 true
constexpr auto TimerSpeedDelayMS = 30; ///< Period of reading speed (Period of TIMER_1 interrupts)



// User defined data types:
enum class MainState { Setup, Speed, Stop }; //enum class is better cause the name of the enum elements can be used outside the enum as variables
enum class CommState { Stop, Wait, SpeedPWM, SpeedReal, Unknown};
using Pin = const uint8_t;

/**
 * \brief Class for variables storage (only header, without methods) 
 */
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
	 static const int address = 0x11;

// Regulator parameters
	 const int motor1DeadBand[2] = { 10,10 }; // [forward,backward] - What is the minimum PWM value on which Motor 1 starts rotating
	 const int motor2DeadBand[2] = { 10,10 }; // [forward,backward] - What is the minimum PWM value on which Motor 2 starts rotating
// Non const variables - will be changed during program
	 
	 MainState actualState = MainState::Setup; ///< Define actual state in which the driver operates 
	 CommState commState = CommState::SpeedPWM; ///< Define communication state in which controller wants the driver to be in, only changed in Comm

	 volatile int actualSpeed[2] = { 0,0 }; ///< Actual speed in PWM of two motors which is sent by analogWrite, (0 - 255)
	 int requiredSpeed[2] = { -50,-50 }; ///< Required speed of two motors by Controler (recieved through I2C), write only in Comm class! (0 - 255)
	 volatile unsigned int encSpeed[2] = { 0,0 }; ///< Actual number of encoder pulses updated every period of reading speed
	 volatile float actualRealSpeed[2] = { 0,0 }; ///< Actual real speed of two motors in rad/s, write only in TimerSpeedHandler interrupt routine
	 float requiredRealSpeed[2] = { 0,0 }; ///< Required real speed of two motors in rad/s by Controler (recieved through I2C), write only in Comm class!
};

//extern StateClass State;

#endif

/* Garbage:
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
