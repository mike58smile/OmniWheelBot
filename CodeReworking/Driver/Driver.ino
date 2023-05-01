/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "State.h" // Include before other classes!
#include "Comm.h"
#include "Drive.h"
#include "SerialControl.h"

#define SerialCtrl

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
#ifdef SerialCtrl
	SerialControl.loop(); //always before Drive.loop
#endif // SerialCtrl

	Drive.loop();
}
