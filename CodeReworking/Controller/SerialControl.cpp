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
        else if (SerialString == "kp1")
            serialMode = SerialMode::Kp1;
        else if (SerialString == "ki1")
            serialMode = SerialMode::Ki1;
        else if (SerialString == "kd1")
            serialMode = SerialMode::Kd1;
        else if (SerialString == "real")
            serialMode = SerialMode::RealSpeed;
        else if (SerialString == "enc")
            serialMode = SerialMode::EncSpeed;
        else if (SerialString == "calib")
            serialMode = SerialMode::CalibDeadBand;
        else if (SerialString == "meas1")
            serialMode = SerialMode::Meas1;
        else if (SerialString == "meas2")
            serialMode = SerialMode::Meas2;
        else if (SerialString == "calc")
            serialMode = SerialMode::CalcSpd;
        else if (SerialString == "calcV")
            serialMode = SerialMode::CalcV;
        else if (SerialString == "circ")
            serialMode = SerialMode::Circle;
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
                Comm.SetPID(1,1,1);
                break;
            default:
                break;
            }
            break;
        case SerialMode::Speed:
        {
            State.state_movement = State_movement::IR_movement;
            int speed = SerialInt;
            if (abs(speed) > 200) speed = sign(speed) * 200;
            Comm.SetPWM(speed, speed, speed, speed);
            break;
        }
        case SerialMode::Kp1:
            Comm.SetPID(SerialFloat, 0, 0);
            break;
        case SerialMode::Ki1:
            Comm.SetPID(0, SerialFloat, 0);
            break;
        case SerialMode::Kd1:
            Comm.SetPID(0, 0, SerialFloat);
            break;
        case SerialMode::RealSpeed:
            State.state_movement = State_movement::IR_movement;
            Comm.SetReal(SerialFloat, 0, 0, 0);
            break;
        case SerialMode::EncSpeed:
            State.state_movement = State_movement::IR_movement;
            Comm.SetRealEnc(SerialFloat, 0, 0, 0);
            break;
        case SerialMode::Meas1:
           
            break;
        case SerialMode::CalcSpd:
            State.state_movement = State_movement::CalcSpd;
            State.wantedAlfa = SerialInt;
            break;
        case SerialMode::CalcV:
            State.wantedV = SerialFloat;
            break;
        case SerialMode::Circle:
            State.state_movement = State_movement::Circle;
            break;
        default:
            break;
        }
    } //here goes breakpoint when debugging - Actions: //enc1={State.encSpeed[0]}, enc2={State.encSpeed[1]}, {Drive.enc1.read()} {State.actualRealSpeed[0]} {State.actualRealSpeed[1]} {State.actualSpeed[0]} {State.actualSpeed[1]} {State.requiredSpeed[0]} {State.requiredSpeed[1]} {State.CommStatePrint[static_cast<int>(State.commState)]} {static_cast<int>(State.commState)} {SerialString} {SerialInt} {EnableSerialMode}
}
SerialControlClass SerialControl(State);

