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
#include "Comm.h"
//#include "Motors.h"
#include "Drive.h"
//#include <Encoder.h>

#include <ISR_Timer.h>
#include <TimerInterrupt.hpp>
#include <TimerInterrupt.h>
#include <ISR_Timer.hpp> 

//StateClass State;
//CommClass Comm(State);
CommClass Comm;
//DriveClass Drive(State);
DriveClass Drive;

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


void setup() {
    // Timer init
    ITimer1.init();
    if (ITimer1.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler))
    {
        Serial.print(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
    }
    else
        Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

	Comm.init();
	Drive.init();
}

/**
 * \brief The loop function which runs over and over again until power down or reset
 */
void loop() {
	Drive.loop();
}
