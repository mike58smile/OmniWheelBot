/*****************************************************************//**
 * \file   Drive.cpp
 * \brief  Drive class source file
 * \details Here are all movements -> it control motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Drive.h"

void DriveClass::init()
{
    Motors.init();

    // Timer init
    ITimer1.init();
    if (ITimer1.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler))
    {
        Serial.print(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
    }
    else
        Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
}
void TimerSpeedHandler()
{
    State.encSpeed[0] = abs(Drive.enc1.readAndReset());
    State.encSpeed[1] = abs(Drive.enc2.readAndReset());
    State.realSpeed[0] = (State.encSpeed[0] * 2*PI) / (979.2 * (TimerSpeedDelayMS/1000));
    State.realSpeed[1] = (State.encSpeed[1] * 2 * PI) / (979.2 * (TimerSpeedDelayMS / 1000));
}
void DriveClass::loop()
{
    
    switch(State.commState){
    case Stop:
        Motors.Stop();
        enc1.write(0);
        enc2.write(0);
        break;
    case Wait:
        break;
    case Speed:
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    default:
        break;
    }
}

void DriveClass::getSpeed()
{
    volatile unsigned long encoderRead[2] = { 0,0 };
    float speed[2] = { 0,0 };
}
DriveClass Drive;

