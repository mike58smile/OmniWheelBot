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
    if (State.state_commPrev != State.state_comm)
        State.state_commPrev = State.state_comm;
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
        else if (SerialString == "meas1")
            serialMode = SerialMode::Meas1;
        else if (SerialString == "meas2")
            serialMode = SerialMode::Meas2;
        else if (SerialString == "get") //Not working now
            serialMode = SerialMode::EnableSerialGet;
        
        switch (serialMode) {
        case SerialMode::Comm:
            switch (SerialInt) {
            case 0:
                //stop motor (and reset PID)
                State.state_comm = State_comm::Stop;
                break;
            case 1:
                State.state_comm = State_comm::Wait;
                break;
            case 2:
                //set speed
                State.requiredSpeed[0] = 50;
                State.requiredSpeed[1] = 50;
                State.state_comm = State_comm::SpeedPWM;
                break;
            case 3:
                State.requiredEncSpeed[0] = 20;
                State.requiredEncSpeed[1] = 20;
                State.state_comm = State_comm::SpeedReal;
                break;
            case 4:
                State.Kp_1 = 5;
                State.state_comm = State_comm::ChangeConstPID;
                break;
            case 5:
                State.state_comm = State_comm::Unknown;
                break;
            default:
                break;
            }
            break;
        case SerialMode::CalibDeadBand:
            State.meas.state_measType = State_measType::Calib;
            State.state_comm = State_comm::Meas;
            break;
        case SerialMode::Kp1:
            State.Kp_1 += SerialFloat;
            State.state_comm = State_comm::ChangeConstPID;
            break;
        case SerialMode::Ki1:
            State.Ki_1 += SerialFloat;
            State.state_comm = State_comm::ChangeConstPID;
            break;
        case SerialMode::Kd1:
            State.Kd_1 += SerialFloat;
            State.state_comm = State_comm::ChangeConstPID;
            break;
        case SerialMode::RealSpeed:
            State.requiredEncSpeed[0] = SerialInt;
            State.requiredEncSpeed[1] = SerialInt;
            State.state_comm = State_comm::SpeedReal;
            break;
        case SerialMode::Meas1:
            //State.state_comm = State_comm::Meas1;
            State.meas.state_measType = State_measType::Ramp;
            State.state_comm = State_comm::Meas;
            break;
        case SerialMode::Meas2:
            State.meas.state_measType = State_measType::Ramp_optim;
            State.state_comm = State_comm::Meas;
            break;
        case SerialMode::EnableSerialGet: //Now doing nothing - big delay when added condition to breakpoint - UNUSABLE
            SerialGetEN = !SerialGetEN;
            break;
        case SerialMode::Speed:
        {
            int speed = SerialInt;
            if (abs(speed) > 200) speed = sign(speed) * 200;
            State.requiredSpeed[0] = speed;
            State.requiredSpeed[1] = speed;
            State.state_comm = State_comm::SpeedPWM; // need to asign State.state_comm for drive to catch the request for changing speed to required
            break;
        }
        default:
            break;
        }
    } //here goes breakpoint when debugging - Actions: //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {Drive.enc1.read()} {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.stateArr_comm[static_cast<int>(State.state_comm)]} {static_cast<int>(State.state_comm)} {SerialString} {SerialInt} {EnableSerialMode}
    //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.requiredRealSpeed[0]} {State.requiredRealSpeed[1]} {State.stateArr_comm[static_cast<int>(State.state_comm)]} {static_cast<int>(State.state_comm)} {SerialString} {SerialInt} {State.Kp_1} {SerialFloat} {SerialModePrint[static_cast<int>(serialMode)]} {State.motor1DeadBand[0]}
}
SerialControlClass SerialControl{};

