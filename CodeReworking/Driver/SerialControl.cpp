/*****************************************************************//**
 * \file   SerialControl.cpp
 * \brief  SerialControl class source file
 * \details Used for controlling (debugging/testing) the robot through Serial
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "SerialControl.h"

void SerialControlClass::init()
{

}

void SerialControlClass::loop()
{
    if (Serial.available() > 0) {
        SerialString = Serial.readStringUntil('\n');
        SerialInt = SerialString.toInt();
        if (SerialString == "com"|| SerialString == "comm")
            EnableSerialMode = 'c';
        else if (SerialString == "spd" || SerialString == "speed")
            EnableSerialMode = 's';
        switch (EnableSerialMode) {
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
    } //here goes breakpoint when debugging - Actions: //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {Drive.enc1.read()} {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.CommStatePrint[static_cast<int>(State.commState)]} {static_cast<int>(State.commState)} {SerialString} {SerialInt} {EnableSerialMode}
}
SerialControlClass SerialControl(State);

