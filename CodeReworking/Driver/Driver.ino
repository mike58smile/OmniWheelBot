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

//CommClass Comm(State);
//DriveClass Drive(State);

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
	if (Serial.available() > 0) {
        char SerialRead = Serial.read();
		switch (SerialRead) {
        case '0':
            //stop motor (and reset PID)
            State.commState = CommState::Stop;
            break;
        case '1':
            State.commState = CommState::Wait;
            break;
        case '2':
            //set speed
            State.requiredSpeed[0] = 30;
            State.requiredSpeed[1] = 30;
            State.commState = CommState::SpeedPWM;
            break;
        case '3':
            State.requiredRealSpeed[0] = WireRead;
            State.requiredRealSpeed[1] = WireRead;
            State.commState = CommState::SpeedReal;
        default:
            State.commState = CommState::Unknown;
            break;
		}
	}
    Drive.loop();
}
