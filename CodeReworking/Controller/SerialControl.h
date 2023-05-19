/*****************************************************************//**
 * \file   SerialControl.h
 * \brief  SerialControl class header
 * \details Used for controlling (debugging/testing) the robot through Serial
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#ifndef _SERIALCONTROL_h
#define _SERIALCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "State.h"
#include "Comm.h"

enum class SerialMode { Idle, Comm, Speed, RealSpeed, EncSpeed, Kp1, Ki1, Kd1, CalibDeadBand, Meas1, Meas2, CalcSpd, CalcV, Circle, Size }; ///< Enum for selected serial mode state machine, "Size" is a little trick - contains number of elements in this enum
class SerialControlClass
{
protected:
	SerialMode serialMode = SerialMode::Idle; ///< Enum for Deadband calibration state machine 
	StateClass& State;
	String SerialString = "";
	int SerialInt = 0;
	float SerialFloat = 0;
public:
	/**
	 * \brief C'tor from StateClass
	 * \param state Reference to storage for all shared variables
	 */
	SerialControlClass(StateClass& state) : State(state) {}

	/**
	 * \brief Initialize Serial controlling - EMPTY
	 * \note Use in void setup()
	 */
	void init();

	/**
	 * \brief Wait for Serial available (data is sent through serial), reads data and change the operating state and state variables
	 * \note Use in void loop()
	 */
	void loop();
};

extern SerialControlClass SerialControl;

#endif
