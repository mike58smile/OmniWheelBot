/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "SerialControl.h"
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
int zmena;
char EnableSerialMode = 'x';
String SerialString = "";
int SerialInt = 0;
/**
 * \brief The loop function which runs over and over again until power down or reset
 */
void loop() {  
 
	if (Serial.available() > 0) {
        SerialString = Serial.readStringUntil('\n');
        SerialInt = SerialString.toInt();
        if (SerialString == "comm")
            EnableSerialMode = 'c';
        else if (SerialString == "spd")
            EnableSerialMode = 's';
        switch(EnableSerialMode){
        case 'c':
            switch (SerialInt) {
            case 0:
                //stop motor (and reset PID)
                State.commState = CommState::Stop;
                break;
            case 1:
                State.commState = CommState::Wait;
                break;
            case 2:
                //set speed
                State.requiredSpeed[0] = 50;
                State.requiredSpeed[1] = 50;
                State.commState = CommState::SpeedPWM;
                break;
            case 3:
                State.requiredRealSpeed[0] = 10;
                State.requiredRealSpeed[1] = 10;
                State.commState = CommState::SpeedReal;
                break;
            case 4:
                State.commState = CommState::Unknown;
                break;
            default:
                break;
            }
            break;
        case 's':
            int speed = SerialInt;
            if (abs(speed) > 200) speed = sign(speed) * 200;
            State.requiredSpeed[0] = speed;
            State.requiredSpeed[1] = speed;
            State.commState = CommState::SpeedPWM; // need to asign commState for drive to catch the request for changing speed to required
            break;
        default:
            break;
		}
	}
    
	Drive.loop();
}
