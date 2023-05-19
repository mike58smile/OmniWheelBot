/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

/**Handy notes:
 * Shortcuts:
 * Ctrl+M+S - collapsing region
 * Ctrl+M+E - expanding region
 * Ctrl+Tab - go back to last opened file
 * Ctrl+(-) - go back to last important place in code (VS somehow determins the importance
 * Learning stuff:
 * Constexpr - lacks pointer to value, means almost nothing to this compiller 
 * Global values shouldnt be auto, short int = int in Arduino
 * enum class is better cause the name of the enum elements can be used outside the enum as variables
 */

/**
 * IMPORTANT:
 * [PWM] is a int value in range <-255 - 255> -> however sometimes only absolut value is allowed, that means <0 - 255>
 */
/**
 * TO DO:
 * Optimize reading speed from encoders - maybe use <stdfix.h> lib for calculations with float numbers
 */

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
	//Serial.println(State.Kp_1);
	//Serial.println(State.tempMode);
	//Serial.println("$"+String(millis())+" "+String(State.actualEncSpeed[0]) + " " + String(State.requiredEncSpeed[0]) + " " + String(Drive.pid_Out1) + " " + String(State.Kp_1) + " " + String(State.Kd_1) + " " + String(State.Ki_1) + "; ");
//Serial.println("$" + String(State.readSpeed) + " " + String(State.actualEncSpeed[0]) + " " + String(State.actualSpeed[0])+ " " + String(State.requiredSpeed[0]) + ";");
	
	//debuging Spds
//Serial.print("$" + String(State.readSpeed) + " " + String(State.actualEncSpeed[0]) + " " + String(State.actualSpeed[0]) + " " + String(State.requiredSpeed[0]) + " next  ");
	//Serial.println(String(State.actualEncSpeed[1]) + " " + String(State.actualSpeed[1]) + " " + String(State.requiredSpeed[1]) + ";");
}
