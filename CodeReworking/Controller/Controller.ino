/*****************************************************************//**
 * \file   Controller.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

// the setup function runs once when you press reset or power the board

#include "Gyro.h"
#include "State.h"
#include "Comm.h"
#include "IR.h"

void setup() {
	Comm.init();
	IR.init();
}

// the loop function runs over and over again until power down or reset
void loop() {
	Comm.SerialDebug();
	IR.control();
	Comm.loop();
}
