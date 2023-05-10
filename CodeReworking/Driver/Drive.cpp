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
    
    //Init PID on both motors
    pid1.SetMode(AUTOMATIC);
    pid1.SetOutputLimits(-80, 80); //Set max output of PID action

    pid2.SetMode(AUTOMATIC);
    pid2.SetOutputLimits(-80, 80); //Set max output of PID action
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
        pid_In1 = State.actualRealSpeed[0];
        pid_Set1 = State.requiredRealSpeed[0];
        pid1.Compute();
        Motors.Speed(roundf(pid_Out1), 0);
        //use pid to set real speed
        break;
    case CommState::CalibDeadBand:
        //predTym musia motory stat
        if (State.CalibEnd) break;
        if (State.actualRealSpeed[0] < 1) {
            State.CalibEnd = 0;
            currentTime_C = micros();
            if ((currentTime_C - previousTime_C) >= SpeedRampDelayCalib) {
                previousTime_C = currentTime_C;
                ++State.requiredSpeed[0];
            }
            Motors.Speed(State.requiredSpeed[0], 0);
        }
        else {
            State.motor1DeadBand[0] = State.actualSpeed[0];
            Motors.Stop();
            State.CalibEnd = 1;
        }
        break;
    case CommState::ChangeConstPID:
        pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1);
        break;
    default:
        break;
    }
}

DriveClass Drive(State);