/*****************************************************************//**
 * \file   SerialControl.cpp
 * \brief  SerialControl class source file
 * \details Used for controlling (debugging/testing) the robot through Serial
 * 
 * \author xmisko06
 * \date   May 2023
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
        SerialFloat = SerialString.toFloat();

        if (SerialString == "com" || SerialString == "comm")
            serialMode = SerialMode::Comm;
        else if (SerialString == "spd" || SerialString == "speed")
            serialMode = SerialMode::Speed;

        switch (serialMode) {
        case SerialMode::Comm:
            switch (SerialInt) {
            case 0:
                //Stop motor (and reset PID)
                Comm.Stop();
                break;
            case 1:
                State.controlState = ControlState::Wait;
                break;
            case 2:
                //set speed
                Comm.SetPWM(50, 50, 50, 50);
                break;
            case 4:
                Comm.SetPID(-0.5, 1.56, 40);
                break;
            default:
                break;
            }
            break;
        case SerialMode::Speed:
        {
            int speed = SerialInt;
            if (abs(speed) > 200) speed = sign(speed) * 200;
            Comm.SetPWM(speed, speed, speed, speed);
            break;
        }
        default:
            break;
        }
    } //here goes breakpoint when debugging - Actions: //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {Drive.enc1.read()} {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.CommStatePrint[static_cast<int>(State.commState)]} {static_cast<int>(State.commState)} {SerialString} {SerialInt} {EnableSerialMode}
}
SerialControlClass SerialControl(State);

