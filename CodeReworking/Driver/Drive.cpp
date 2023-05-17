/*****************************************************************//**
 * \file   Drive.cpp
 * \brief  Drive class source file, also creates an object called Drive
 * \details Here are all movements -> it control motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Drive.h"

////Initialize static class members
//unsigned long DriveClass::currentTime = 0;
//unsigned long DriveClass::previousTime = 0;

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
        //Serial.println(currentTime - previousTime);
        State.actualEncSpeed[0] = enc1.readAndReset();
        State.actualEncSpeed[1] = enc2.readAndReset();
        State.readSpeed = currentTime - previousTime; //Save reading speed
       // State.actualRealSpeed[0] = EncToRealSpd(State.actualEncSpeed[0]);
        //State.actualRealSpeed[1] = EncToRealSpd(State.actualEncSpeed[1]);
        pid_In1 = State.actualEncSpeed[0];
        pid_Set1 = State.requiredEncSpeed[0];
        previousTime = currentTime;
        if (!pid_In1 && !pid_Set1) {
            pid1.SetMode(MANUAL);
            pid_Out1 = 0;
        }
        else pid1.SetMode(AUTOMATIC);
        pid1.Compute();
    }
}


inline int DriveClass::PWMtoOptimizedPWM(int PWMspeed, int zeroVal, int minPWM)
{
    if (abs(roundf(PWMspeed)) <= zeroVal)
        return 0;
    else
       return sign(roundf(PWMspeed)) * (abs(roundf(PWMspeed)) + minPWM);
}

void DriveClass::rampInit(bool motSelect, int TimeSlope, int SpeedBegin)
{
    State.requiredSpeed[motSelect] = SpeedBegin;
    Timer_ramp[motSelect]->stop();
    Timer_ramp[motSelect]->setTimeout(TimeSlope);
    Timer_ramp[motSelect]->start();
    rampInitDone[motSelect] = true;
}

void DriveClass::rampUpdate(bool motSelect, int increment, bool optim)
{
    if (!rampInitDone[motSelect] || !increment) return;

    Timer_ramp[motSelect]->update();
    if (Timer_ramp[motSelect]->isStopped()) {
        Timer_ramp[motSelect]->start();
        State.requiredSpeed[motSelect] += increment;
        if(optim)
            Motors.SpeedSingle(motSelect, PWMtoOptimizedPWM(State.requiredSpeed[motSelect], 3, 20));
        else
            Motors.SpeedSingle(motSelect, State.requiredSpeed[motSelect]);
    }
}


void DriveClass::AccTillRotating_init(bool motSelect, int TimeSlope)
{
    Motors.SpeedSingle(motSelect, 0);
    rampInit(motSelect, TimeSlope);
    accTillRotatingDone[motSelect] = false;
}

bool DriveClass::AccTillRotating_update(bool motSelect, int increment, unsigned int endSpeed = 0)
{
    if (!accTillRotatingDone[motSelect]) {
        if (State.actualEncSpeed[motSelect] <= endSpeed) {
            rampUpdate(motSelect, increment);
        }
        else {
            State.motor1DeadBandPWM[motSelect] = State.actualSpeed[motSelect];
            State.motor1DeadBandReal[motSelect] = State.actualEncSpeed[motSelect];
            Serial.println("Speed of motor " + String(motSelect) + " after acceleration when start moving : [PWM] = " + String(State.motor1DeadBandPWM[motSelect]) + ", [ENC] = "+String(State.motor1DeadBandReal[motSelect]));
            accTillRotatingDone[motSelect] = true;
        }
    }
    return accTillRotatingDone[motSelect];
}

void DriveClass::DeccTillRotating_init(bool motSelect, int timeSlope, int speedBegin)
{
    if (speedBegin) {
        Motors.SpeedSingle(motSelect, speedBegin);
    }
    rampInit(motSelect, timeSlope, State.actualSpeed[motSelect]);
    deccTillRotatingDone[motSelect] = false;
}

bool DriveClass::DeccTillRotating_update(bool motSelect, unsigned int decrement, unsigned int endSpeed = 0)
{
    if (!deccTillRotatingDone[motSelect]) {
        if (State.actualEncSpeed[motSelect] > endSpeed) {
            rampUpdate(motSelect, -1*sign(State.actualSpeed[motSelect])*decrement);
        }
        else {
            State.motor2DeadBandPWM[motSelect] = State.actualSpeed[motSelect];
            State.motor2DeadBandReal[motSelect] = State.actualEncSpeed[motSelect];
            Serial.println("Speed of motor " + String(motSelect) + " after decceleration when stop moving : [PWM] = " + String(State.motor2DeadBandPWM[motSelect]) + ", [ENC] = " + String(State.motor2DeadBandReal[motSelect]));
            deccTillRotatingDone[motSelect] = true;
        }
    }
    return deccTillRotatingDone[motSelect];
}
void DriveClass::AccTillPWM_init(bool motSelect, int TimeSlope)
{
    rampInit(motSelect, TimeSlope);
    accTillPWMDone[motSelect] = false;
}
void DriveClass::AccTillPWM_init(bool motSelect, int TimeSlope, int speedBegin)
{
    Motors.SpeedSingle(motSelect, speedBegin);
    rampInit(motSelect, TimeSlope, speedBegin);
    accTillPWMDone[motSelect] = false;
}

bool DriveClass::AccTillPWM_update(bool motSelect, int increment, int endSpeed, bool optim)
{
    if (!accTillPWMDone[motSelect]) {
        if (State.actualSpeed[motSelect] < endSpeed)
            rampUpdate(motSelect, increment, optim);
        else 
            accTillPWMDone[motSelect] = true;
    }
    return accTillPWMDone[motSelect];
}

void DriveClass::DeccTillPWM_init(bool motSelect, int timeSlope, int speedBegin)
{
    Motors.SpeedSingle(motSelect, speedBegin);
    rampInit(motSelect, timeSlope, speedBegin);
    deccTillPWMDone[motSelect] = false;
}

void DriveClass::DeccTillPWM_init(bool motSelect, int timeSlope)
{
    rampInit(motSelect, timeSlope, State.actualSpeed[motSelect]);
    deccTillPWMDone[motSelect] = false;
}

bool DriveClass::DeccTillPWM_update(bool motSelect, unsigned int decrement, int endSpeed, bool optim)
{
    if (!deccTillPWMDone[motSelect]) {
        if (State.actualSpeed[motSelect] > endSpeed)
            rampUpdate(motSelect, -decrement, optim);
        else
            deccTillPWMDone[motSelect] = true;
    }
    return deccTillPWMDone[motSelect];
}

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
        break;
    case CalibState::Motor1_f: //Calibrate first motor - Increment PWM on motor until it starts rotating (the real speed from encoders is bigger than 1 rad/s)
    {
        bool ret = AccTillRotating_update(1, 1, 1);
        ret = AccTillRotating_update(0, 1, 1) && ret;
        if (ret)
            calibState = CalibState::Motor1_b;
        break;
    }
    case CalibState::Motor1_b:
        DeccTillRotating_init(0, SpeedRampDelayCalib);
        DeccTillRotating_init(1, SpeedRampDelayCalib);
        calibState = CalibState::Motor2_f;
        break;
    case CalibState::Motor2_f:
    {
        bool ret2 = DeccTillRotating_update(0, 1, 1);
        ret2 = DeccTillRotating_update(1, 1, 1) && ret2;
        if (ret2)
            calibState = CalibState::End;
        break;
    }
    case CalibState::End: //End of calibration of both motors - indicated by State.CalibEnd making true - After first calibration it stays in this state forever
        Motors.Stop();
        State.CalibEnd = 1;
        break;
    }
    return State.CalibEnd;
}

bool DriveClass::LinearityRamp_meassure(int maxSpeed, int motSelect, bool optim) {
    switch (linearityRamp_state) {
    case LinearityRamp_state::InitAcc:
        AccTillPWM_init(motSelect, SpeedRampDelayCalib,-maxSpeed);
        linearityRamp_state = LinearityRamp_state::Acc;
        break;
    case LinearityRamp_state::Acc:
        if(AccTillPWM_update(motSelect, 1, maxSpeed, optim))
            linearityRamp_state = LinearityRamp_state::InitDecc;
        break;
    case LinearityRamp_state::InitDecc:
        DeccTillPWM_init(motSelect, SpeedRampDelayCalib);
        linearityRamp_state = LinearityRamp_state::Decc;
        break;
    case LinearityRamp_state::Decc:
        if(DeccTillPWM_update(motSelect, 1, 0, optim))
            linearityRamp_state = LinearityRamp_state::End;
        break;
    case LinearityRamp_state::End:
        return true;
        break;
    }
    return false;
}

int actualPidOut1 = 0;
int deadbandPWM = 20;

void DriveClass::loop()
{
    read(); //Update speed of motors - reads encoders and writes speed of motors to State variables
    
    //Read from commState and determine in which state controller wants driver to be in
    switch(State.commState){
    case CommState::Stop:
        Motors.Stop();
        //enc1.write(0); //odstranit probably
        enc2.write(0);//odstranit probably
        break;
    case CommState::Wait:
        calibState = CalibState::Init;
        linearityRamp_state = LinearityRamp_state::InitAcc;
        break;
    case CommState::SpeedPWM: 
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    case CommState::SpeedReal:
        //pid_In1 = State.actualRealSpeed[0];
        //pid_Set1 = State.requiredRealSpeed[0];
        //pid1.Compute();
        //if (abs(roundf(pid_Out1)) < (State.motor1DeadBand[0] - 10))
        //    pid_Out1 = sign(pid_Out1) * (State.motor1DeadBand[0] - 10);
        // 
        
        if (abs(roundf(pid_Out1)) <= 5)
            actualPidOut1 = 0;
        else
            actualPidOut1 = sign(roundf(pid_Out1))*(abs(roundf(pid_Out1)) + deadbandPWM);
        Motors.Speed(actualPidOut1, 0);

        //Motors.Speed(roundf(pid_Out1), 0);
        //use pid to set real speed
        break;
    case CommState::Meas:
        switch (State.meas.measType) {
        case MeasType::Calib:
            if(CalibDeadband()) State.commState = CommState::Wait;
            break;
        case MeasType::Ramp:
            if(LinearityRamp_meassure(70, State.meas.motSelect, 0)) State.commState = CommState::Wait;
            //LinearityRamp_meassure(70, State.meas.motSelect, 0);
            break;
        case MeasType::Ramp_optim:
            if(LinearityRamp_meassure(50,State.meas.motSelect,1)) State.commState = CommState::Wait;
            //LinearityRamp_meassure(50, State.meas.motSelect, 1);
            break;
        }
        break;
    case CommState::CalibDeadBand:
        CalibDeadband();
        break;
    case CommState::Meas1:
        LinearityRamp_meassure(50, State.meas.motSelect, 1);
        break;
    case CommState::ChangeConstPID: //Change PID constants
        pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1); //Constants should be already updated
        State.commState = State.commStatePrev; //So it is here only once
        break;
    default:
        break;
    }//{roundf(pid_Out1)} {commStatePrint}
}

DriveClass Drive{};

//Garbage:
    //case CalibState::Motor1_b:
    //    Serial.println("in 1_b");

    //    //Motors.Stop();

    //    //AccTillRotating_init(0, SpeedRampDelayCalib);
    //    //AccTillRotating_init(1, SpeedRampDelayCalib);

    //    //DeccTillRotating_init(0, SpeedRampDelayCalib);
    //    //DeccTillRotating_init(1, SpeedRampDelayCalib);
    //    calibState = CalibState::Motor2_f;
    //    break;
    //case CalibState::Motor2_f:
    //    Serial.println("in case");
    //    //calibState = CalibState::End;
    //    //bool ret2 = DeccTillRotating_update(1, 1, 1);
    //    //ret2 = DeccTillRotating_update(0, 1, 0) && ret;
    //    //if (ret2)
    //    //    calibState = CalibState::End;

    //    //bool ret2 = AccTillRotating_update(1, 1, 1);
    //    //ret2 = AccTillRotating_update(0, 1, 1) && ret2;
    //    //if (ret2)
    //    //    calibState = CalibState::End;
    //    break;
