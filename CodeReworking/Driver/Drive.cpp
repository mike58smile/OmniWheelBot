/*****************************************************************//**
 * \file   Drive.cpp
 * \brief  Drive class source file, also creates an object called Drive
 * \details Here are all movements -> it control motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Drive.h"


//Initialize static class members
unsigned long DriveClass::currentTime = 0;
unsigned long DriveClass::previousTime = 0;

void DriveClass::init()
{
    Motors.init();
}
void DriveClass::read()
{
    currentTime = micros();
    if ((currentTime - previousTime) >= TimerSpeedDelay_uS) {
        State.encSpeed[0] = abs(enc1.read());
        enc1.write(0);
        State.encSpeed[1] = abs(enc2.read());
        enc2.write(0);
        State.actualRealSpeed[0] = EncToRealSpd(State.encSpeed[0]);
        State.actualRealSpeed[1] = EncToRealSpd(State.encSpeed[1]);
        previousTime = currentTime;
    }
}
void DriveClass::loop()
{
    read();
    switch(State.commState){
    case CommState::Stop:
        Motors.Stop();
        enc1.write(0);
        enc2.write(0);
        break;
    case CommState::Wait:
        break;
    case CommState::SpeedPWM:
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    case CommState::SpeedReal:
        //use pid to set real speed
        break;
    default:
        break;
    }
}

DriveClass Drive(State);