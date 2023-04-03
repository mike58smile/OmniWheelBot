/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

//#include <Wire.h>
//#include "State.h" // Include before Drive.h!
#include "Wire.h"
#include "Comm.h"
//#include "Motors.h"
#include "Drive.h"
//#include <Encoder.h>

#include <ISR_Timer.h>
#include <TimerInterrupt.hpp>
#include <TimerInterrupt.h>
#include <ISR_Timer.hpp> 

constexpr auto BaudRate = 115200;
#define WireRead (Wire.read() | Wire.read() << 8)

#define WireWrite(int16){ \
Wire.write(int16); \
Wire.write((int16 >> 8)); \
}

StateClass State;
CommClass Comm(State);
DriveClass Drive(State);
//CommClass Comm;
//DriveClass Drive;

void TimerSpeedHandler()
{
    State.encSpeed[0] = abs(Drive.enc1.readAndReset());
    State.encSpeed[1] = abs(Drive.enc2.readAndReset());
    State.actualRealSpeed[0] = (State.encSpeed[0] * 2 * PI) / (979.2 * (TimerSpeedDelayMS / 1000));
    State.actualRealSpeed[1] = (State.encSpeed[1] * 2 * PI) / (979.2 * (TimerSpeedDelayMS / 1000));
}
/**
 * \brief the setup function runs once when you press reset or power the board
 */

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



void setup() {
    // Timer init
    ITimer1.init();
    if (ITimer1.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler))
    {
        Serial.print(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
    }
    else
        Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
    Serial.begin(BaudRate);
    Wire.begin(State.address);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveData);

	//Comm.init();
	Drive.init();
}

/**
 * \brief The loop function which runs over and over again until power down or reset
 */
void loop() {
	Drive.loop();
}
