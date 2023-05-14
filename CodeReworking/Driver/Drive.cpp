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

void DriveClass::rampInit(bool motSelect, int TimeSlope, int SpeedBegin)
{
    State.requiredSpeed[motSelect] = SpeedBegin;
    Timer_ramp[motSelect]->setTimeout(TimeSlope);
    Timer_ramp[motSelect]->start();
    rampInitDone[motSelect] = true;
}

void DriveClass::rampUpdate(bool motSelect, int increment)
{
    if (!rampInitDone[motSelect] || !increment) return;

    Timer_ramp[motSelect]->update();
    if (Timer_ramp[motSelect]->isStopped()) {
        Timer_ramp[motSelect]->start();
        State.requiredSpeed[motSelect] += increment;
        Motors.SpeedSingle(motSelect, State.requiredSpeed[motSelect]);
    }
}
void DriveClass::AccTillRotating_init(bool motSelect, int TimeSlope)
{
    Motors.SpeedSingle(motSelect, 0);
    rampInit(motSelect, TimeSlope);
    accTillRotatingDone[motSelect] = false;
}

bool DriveClass::AccTillRotating_update(bool motSelect, unsigned int endSpeed, int increment)
{
    if (!accTillRotatingDone[motSelect]) {
        if (State.actualRealSpeed[motSelect] <= endSpeed) {
            rampUpdate(motSelect, increment);
        }
        else {
            State.motor1DeadBandPWM[0] = State.actualSpeed[motSelect];
            State.motor1DeadBandReal[0] = State.actualRealSpeed[motSelect];
            accTillRotatingDone[motSelect] = true;
        }
    }
    return accTillRotatingDone[motSelect];
}

bool a = 0, b = 0;
bool DriveClass::CalibDeadband() {
    switch (calibState) {
    case CalibState::Init: //Stop motors and reset some variables to zero
        Motors.Stop();
        State.requiredSpeed[0] = 0;
        State.requiredSpeed[1] = 0;
        State.CalibEnd = 0;
        calibState = CalibState::Motor1_f;
        AccTillRotating_init(0, SpeedRampDelayCalib);
        AccTillRotating_init(1, SpeedRampDelayCalib);
        //rampInit(1, SpeedRampDelayCalib);
        break;
    case CalibState::Motor1_f: //Calibrate first motor - Increment PWM on motor until it starts rotating (the real speed from encoders is bigger than 1 rad/s)
        //if (!a) {
        //    if (State.actualRealSpeed[0] < 1) {
        //        rampUpdate(0, 1);
        //    }
        //    else {
        //        State.motor1DeadBandPWM[0] = State.actualSpeed[0];
        //        State.motor1DeadBandReal[0] = State.actualRealSpeed[0];
        //        a = 1;
        //    }
        //}
        //AccTillRotating_update(0, 1, 1);
        bool ret = AccTillRotating_update(1, 1, 1);
        ret = AccTillRotating_update(0, 1, 1) && ret;
        if(ret)
            calibState = CalibState::End;

        //if (!b) {
        //    if (State.actualRealSpeed[1] < 1) {
        //        rampUpdate(1, 1);
        //    }
        //    else {
        //        State.motor2DeadBandPWM[0] = State.actualSpeed[1];
        //        State.motor2DeadBandReal[0] = State.actualRealSpeed[1];
        //        //Motors.SpeedSingle(1, 0);
        //        b = 1;
        //    }
        //}
        //if (a && b) {
        //    calibState = CalibState::End;
        //}
        break;
    //    if (State.actualRealSpeed[0] < 1) {
    //        currentTime_C = micros();
    //        if ((currentTime_C - previousTime_C) >= SpeedRampDelayCalib) {
    //            previousTime_C = currentTime_C;
    //            ++State.requiredSpeed[0];
    //        }
    //        Motors.Speed(State.requiredSpeed[0], 0);
    //    }
    //    else {
    //        State.motor1DeadBandPWM[0] = State.actualSpeed[0];
    //        State.motor1DeadBandReal[0] = State.actualRealSpeed[0];
    //        Motors.Stop();
    //        calibState = CalibState::Motor1_b;
    //    }
    //    break;
    //case CalibState::Motor1_b:
    //    if (State.actualRealSpeed[0] < 1) {
    //        currentTime_C = micros();
    //        if ((currentTime_C - previousTime_C) >= SpeedRampDelayCalib) {
    //            previousTime_C = currentTime_C;
    //            --State.requiredSpeed[0];
    //        }
    //        Motors.Speed(State.requiredSpeed[0], 0);
    //    }
    //    else {
    //        State.motor1DeadBandPWM[1] = State.actualSpeed[0];
    //        State.motor1DeadBandReal[1] = State.actualRealSpeed[0];
    //        Motors.Stop();
    //        calibState = CalibState::Motor2_f;
    //    }
    //    break;
    //case CalibState::Motor2_f: //Calibrate second motor - Increment PWM on motor until it starts rotating (the real speed from encoders is bigger than 1 rad/s)
    //    if (State.actualRealSpeed[1] < 1) {
    //        currentTime_C = micros();
    //        if ((currentTime_C - previousTime_C) >= SpeedRampDelayCalib) {
    //            previousTime_C = currentTime_C;
    //            ++State.requiredSpeed[1];
    //        }
    //        Motors.Speed(0, State.requiredSpeed[1]);
    //    }
    //    else {
    //        State.motor2DeadBandPWM[0] = State.actualSpeed[1];
    //        State.motor2DeadBandReal[0] = State.actualRealSpeed[1];
    //        Motors.Stop();
    //        calibState = CalibState::Motor2_b;
    //    }
    //    break;
    //case CalibState::Motor2_b:
    //    if (State.actualRealSpeed[1] < 1) {
    //        currentTime_C = micros();
    //        if ((currentTime_C - previousTime_C) >= SpeedRampDelayCalib) {
    //            previousTime_C = currentTime_C;
    //            --State.requiredSpeed[1];
    //        }
    //        Motors.Speed(0, State.requiredSpeed[1]);
    //    }
    //    else {
    //        State.motor2DeadBandPWM[1] = State.actualSpeed[1];
    //        State.motor2DeadBandReal[1] = State.actualRealSpeed[1];
    //        Motors.Stop();
    //        calibState = CalibState::End;
    //    }
    //    break;
    case CalibState::End: //End of calibration of both motors - indicated by State.CalibEnd making true - After first calibration it stays in this state forever
        State.CalibEnd = 1;
        break;
    }
    return State.CalibEnd;
}

void DriveClass::loop()
{
    read(); //Update speed of motors - reads encoders and writes speed of motors to State variables
    
    //Read from commState and determine in which state controller wants driver to be in
    switch(State.commState){
    case CommState::Stop:
        Motors.Stop();
        enc1.write(0); //odstranit probably
        enc2.write(0);//odstranit probably
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
        //if (abs(roundf(pid_Out1)) < (State.motor1DeadBand[0] - 10))
        //    pid_Out1 = sign(pid_Out1) * (State.motor1DeadBand[0] - 10);
        Motors.Speed(roundf(pid_Out1), 0);
        //use pid to set real speed
        break;
    case CommState::CalibDeadBand:
        CalibDeadband();
        break;
    case CommState::ChangeConstPID: //Change PID constants
        pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1);
        break;
    default:
        break;
    }//{roundf(pid_Out1)} {commStatePrint}
}

DriveClass Drive(State);