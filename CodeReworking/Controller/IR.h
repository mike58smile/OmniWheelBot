/*****************************************************************//**
 * \file   IR.h
 * \brief  IR class header
 * \details Used for handling recieving IR signal
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#ifndef _IR_h
#define _IR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "State.h"

/**
 * \brief Class handling recieving IR signal
 */
class IRClass
{
 protected:
	 StateClass& State; ///< Storage for all shared variables 
	 //IRrecv Irrecv; ///< Object for recieving IR signals
	 //decode_results results;

	 uint32_t lastRecieved = 0, currentRecievedFlag = 0;
	 bool isSameRecieved = 0;
	 // For Long press detection
	 unsigned long sMillisOfFirstReceive;
	 bool sLongJustPressed; ///<
	 bool LongPressFlag;
	 bool detectLongPress(uint16_t aLongPressDurationMillis);
	 bool read();
 public:
	 /**
	  * \brief C'tor from StateClass, also initialize Irrecv object
	  * \note This is the only constructor
	  * \param state Reference to storage for all shared variables
	  */
	 IRClass(StateClass& state) : State(state) {}

	/**
	* \brief Initialize IR
	* \note Use in void setup()
	*/
	void init();

	uint32_t test();
	void control();
};

extern IRClass IR;

#endif

