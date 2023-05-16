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

//using IRval = constexpr uint32_t;

#define IRval constexpr uint32_t
namespace IR_denon {
	IRval UP = 0xF001B140;
	IRval RIGHT = 0xA001E140;
	IRval DOWN = 0x8001C140;
	IRval LEFT = 0x9001D140;
	IRval ENTER = 0xB001F140;
	IRval VOL_UP = 0x56170140;
	IRval VOL_DOWN = 0x46171140;
	IRval CH_UP = 0xBD3CC140;
	IRval CH_DOWN = 0xAD3CD140;
	IRval NUM_1 = 0x40010140;
	IRval NUM_2 = 0x50011140;
	IRval NUM_3 = 0x60012140;
	IRval NUM_4 = 0x70013140;
	IRval NUM_5 = 0x14140;
	IRval NUM_6 = 0x10015140;
	IRval NUM_7 = 0x40010140;
	IRval NUM_8 = 0x50011140;
	IRval NUM_9 = 0xC0018140;
};
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

