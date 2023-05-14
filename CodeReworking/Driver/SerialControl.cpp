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
   //Serial.print(String(State.actualRealSpeed[0]) + " " + String(State.actualRealSpeed[1]) + " " + String(State.motor1DeadBandReal[0]) + " " + String(State.motor2DeadBandReal[0]));
   //Serial.println();
    if (Serial.available() > 0) {
        SerialString = Serial.readStringUntil('\n');
        SerialInt = SerialString.toInt();
        SerialFloat = SerialString.toFloat();
        //Need if statements because in switch, u can't compare strings ..sad
        if (SerialString == "com" || SerialString == "comm")
            serialMode = SerialMode::Comm;
        else if (SerialString == "spd" || SerialString == "speed")
            serialMode = SerialMode::Speed;
        else if (SerialString == "kp1")
            serialMode = SerialMode::Kp1;
        else if (SerialString == "ki1")
            serialMode = SerialMode::Ki1;
        else if (SerialString == "kd1")
            serialMode = SerialMode::Kd1;
        else if (SerialString == "real")
            serialMode = SerialMode::RealSpeed;
        else if (SerialString == "calib")
            serialMode = SerialMode::CalibDeadBand;
        else if (SerialString == "get") //Not working now
            serialMode = SerialMode::EnableSerialGet;

        switch (serialMode) {
        case SerialMode::Comm:
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
                State.Kp_1 = 5;
                State.commState = CommState::ChangeConstPID;
                break;
            case 5:
                State.commState = CommState::Unknown;
                break;
            default:
                break;
            }
            break;
        case SerialMode::CalibDeadBand:
            State.commState = CommState::CalibDeadBand;
            break;
        case SerialMode::Kp1: //!funguje iba pred Speed
            State.Kp_1 = SerialFloat;
            State.commState = CommState::ChangeConstPID;
            break;
        case SerialMode::Ki1: //!funguje iba pred Speed
            State.Ki_1 = SerialFloat;
            State.commState = CommState::ChangeConstPID;
            break;
        case SerialMode::Kd1: //!funguje iba pred Speed
            State.Kd_1 = SerialFloat;
            State.commState = CommState::ChangeConstPID;
            break;
        case SerialMode::RealSpeed: //!funguje iba pred Speed
            State.requiredRealSpeed[0] = SerialFloat;
            State.requiredRealSpeed[1] = SerialFloat;
            State.commState = CommState::SpeedReal;
            break;
        case SerialMode::EnableSerialGet: //Now doing nothing - big delay when added condition to breakpoint - UNUSABLE
            SerialGetEN = !SerialGetEN;
            break;
        case SerialMode::Speed:
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
    //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.requiredRealSpeed[0]} {State.requiredRealSpeed[1]} {State.CommStatePrint[static_cast<int>(State.commState)]} {static_cast<int>(State.commState)} {SerialString} {SerialInt} {State.Kp_1} {SerialFloat} {SerialModePrint[static_cast<int>(serialMode)]} {State.motor1DeadBand[0]}
}
SerialControlClass SerialControl(State);

