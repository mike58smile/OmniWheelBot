/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

//#include <Wire.h>
#include "State.h" // Include before Drive.h!
#include "Wire.h"
#include "Comm.h"
#include "Drive.h"

constexpr auto BaudRate = 115200;
#define WireRead (Wire.read() | Wire.read() << 8)

#define WireWrite(int16){ \
Wire.write(int16); \
Wire.write((int16 >> 8)); \
}

StateClass State;
CommClass Comm(State);
DriveClass Drive(State);

void requestEvent()
{
    WireWrite(State.actualSpeed[0]);
    WireWrite(State.actualSpeed[1]);
    char strBuffer[7];
    Wire.write(dtostrf(State.actualRealSpeed[0], 7, 2, strBuffer)); //!odskusat ci funguje
    Wire.write(dtostrf(State.actualRealSpeed[1], 7, 2, strBuffer));//!odskusat ci funguje  
}

void receiveData(int x)
{
    int mode = Wire.read();
    switch (mode) {
    case 0:
        //stop motor (and reset PID)
        State.commState = CommState::Stop;
        break;
    case 1:
        State.commState = CommState::Wait;
        break;
    case 2:
        //set speed
        State.requiredSpeed[0] = WireRead;
        State.requiredSpeed[1] = WireRead;
        State.commState = CommState::SpeedPWM;
        break;
    case 3:
        State.requiredRealSpeed[0] = WireRead;
        State.requiredRealSpeed[1] = WireRead;
        State.commState = CommState::SpeedReal;
    default:
        State.commState = CommState::Unknown;
        break;
    }
}


/**
 * \brief the setup function runs once when you press reset or power the board
 */
void setup() {
    Serial.begin(BaudRate);
    Wire.begin(State.address);
	//Comm.init();
   
    Wire.onReceive(receiveData);
    Wire.onRequest(requestEvent);
	Drive.init();
}

/**
 * \brief The loop function which runs over and over again until power down or reset
 */
void loop() {    
	Drive.loop();
}
