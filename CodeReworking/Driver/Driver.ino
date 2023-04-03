/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "State.h" // Include before Drive.h!
#include "Comm.h"
#include "Motors.h"
#include "Drive.h"

#include <ISR_Timer.h>
#include <ISR_Timer.hpp>
#include <TimerInterrupt.h>
#include <TimerInterrupt.hpp>

//DriveClass Drive;

/**
 * \brief the setup function runs once when you press reset or power the board
 */
void setup() {
	Comm.init();
	Drive.init();
}

/**
 * \brief The loop function which runs over and over again until power down or reset
 */
void loop() {
	Drive.loop();
}
