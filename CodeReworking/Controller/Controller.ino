/*****************************************************************//**
 * \file   Controller.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

// the setup function runs once when you press reset or power the board


#include "Movements.h"
#include "Gyro.h"
#include "State.h"
#include "Comm.h"
#include "IR.h"
#include "SerialControl.h"

//#define IR_test
//#define Gyro_test

void setup() {
	Comm.init();
	Movements.init();
	IR.init();
}

// the loop function runs over and over again until power down or reset
void loop() {
#ifdef IR_test
	while(true)
		IR.test();
#endif
#ifdef Gyro_test
	while (true)
		Movements.gyroTest();
#endif
	//Comm.SerialDebug();
	Comm.loop();
	//SerialControl.loop();
	IR.control();
	//Movements.gyroTest();
	Movements.loop();
	//Serial.println("$" + String(millis()) + " " + String(State.actualEncSpeed[0]) + " " + String(State.requiredEncSpeed[0]) + " " + String(State.actualSpeed[0]) + " " + String(State.Kp_1) + " " + String(State.Ki_1) + " " + String(State.Kd_1) + "; ");

}
