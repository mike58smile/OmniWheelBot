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

#include <timer.h>
#include "State.h"

//using IRval = constexpr uint32_t;

#define IRval constexpr uint32_t
namespace IR_denon {
	IRval UP = 0xF001B140;
	IRval RIGHT = 0xA001E140;
	IRval DOWN = 0x8001C140;
	IRval LEFT = 0x9001D140;
	IRval ENTER = 0xB001F140;
	IRval MENU = 0x73023140;
	IRval CH_LEVEL = 0xF617A140;
	IRval RETURN = 0x63022140;
	IRval SEARCH = 0xC3D7140;
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
	IRval NUM_7 = 0x20016140;
	IRval NUM_8 = 0x30017140;
	IRval NUM_9 = 0xC0018140;
	IRval FAST_BACKWARD = 0x4F3E3140;
	IRval PAUSE = 0x5F3E2140;
	IRval STOP = 0x6F3E1140;
	IRval FAST_FORWARD = 0x3F3E4140;
	IRval MUTE = 0x76172140;
	IRval BACKWARD = 0X7C3D0140;
	IRval FORWARD = 0x4C3D3140;
	IRval PLAY = 0x7F3E0140;
	IRval ON_MAIN = 0x61002140;
	IRval STANDBY_MAIN = 0x71003140;
	IRval INTERNET_RADIO = 0x3F2E5140;
	IRval PRESET_1 = 0x6E0F2140;
	IRval PRESET_2 = 0x7E0F3140;
	IRval PRESET_3 = 0xE0F4140;
};

constexpr float add = 0.1;
static int realSpdBegin = 30;
/**
 * \brief Class handling recieving IR signal
 */
class IRClass
{
 protected:
	 StateClass& State; ///< Storage for all shared variables 
	 //IRrecv Irrecv; ///< Object for recieving IR signals
	 //decode_results results;
	 Timer timer;
	 uint32_t lastRecieved = 0, currentRecievedFlag = 0;
	 bool isSameRecieved = 0, isSingleClick = 0;
	 // For Long press detection
	 unsigned long sMillisOfFirstReceive = 0, sMillisOfFirstReceive2 = 0;
	 bool sLongJustPressed = 0, sLongJustPressed2 = 0; ///<
	 bool LongPressFlag;
	 bool detectLongPress(uint16_t aLongPressDurationMillis);
	 bool detectNextPress(uint16_t aLongPressDurationMillis);
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

