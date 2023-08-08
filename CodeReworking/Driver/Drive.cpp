/*****************************************************************//**
 * \file   Drive.cpp
 * \brief  Drive class source file, also creates an object called Drive
 * \details Here are all movements -> it control motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Drive.h"

void DriveClass::init()
{
    Motors.init();
    
    //Init PID on both motors
    pid1.SetMode(AUTOMATIC);
    pid1.SetOutputLimits(-PWMtoOptimizedPWM(maxSpeed), PWMtoOptimizedPWM(maxSpeed)); //Set max output of PID action

    pid2.SetMode(AUTOMATIC);
    pid2.SetOutputLimits(-PWMtoOptimizedPWM(maxSpeed), PWMtoOptimizedPWM(maxSpeed)); //Set max output of PID action
}
void DriveClass::read()
{
    currentTime = micros();
    if ((currentTime - previousTime) >= TimerSpeedDelay_uS) {
        State.readSpeed = currentTime - previousTime; //Save reading speed

        State.actualEncSpeed[0] = enc1.readAndReset();
        State.actualEncSpeed[1] = enc2.readAndReset();
        //Serial.println("$" + String(millis()) + " " + String(State.actualEncSpeed[0]) + " " + String(State.actualSpeed[0]) + "; ");
        pid_In1 = State.actualEncSpeed[0];
        pid_Set1 = State.requiredEncSpeed[0];
        pid1.Compute();

        pid_In2 = State.actualEncSpeed[1];
        pid_Set2 = State.requiredEncSpeed[1];
        pid2.Compute();

        previousTime = currentTime;
    }
}

inline int DriveClass::PWMtoOptimizedPWM(int PWMspeed, unsigned int zeroVal, unsigned int minPWM)
{
    if (abs(PWMspeed) <= zeroVal)
        return 0;
    else
       return sign(PWMspeed) * (abs(PWMspeed) + minPWM - (zeroVal + 1)); //- (zeroVal + 1)
}

void DriveClass::rampInit(bool motSelect, int timeSlope)
{
    //State.requiredSpeed[motSelect] = State.actualSpeed[motSelect]; //added
    Timer_ramp[motSelect]->stop();
    Timer_ramp[motSelect]->setTimeout(timeSlope);
    Timer_ramp[motSelect]->start();
    rampInitDone[motSelect] = true;
}

void DriveClass::rampInit(bool motSelect, int timeSlope, int SpeedBegin, bool optim)
{
    State.requiredSpeed[motSelect] = SpeedBegin; //= optim ? PWMtoOptimizedPWM(SpeedBegin, 3, 20) : SpeedBegin;
    //if (abs(State.requiredSpeed[motSelect]) > 0 && abs(State.requiredSpeed[motSelect]) < 50 && State.actualEncSpeed == 0) {
    //    while (!AccFromZero((!motSelect)? sign(State.requiredSpeed[motSelect]) : 0, (motSelect)? sign(State.requiredSpeed[motSelect]) : 0));
    //}
    Motors.SpeedSingle(motSelect, (optim ? PWMtoOptimizedPWM(State.requiredSpeed[motSelect]) : State.requiredSpeed[motSelect]));
    rampInit(motSelect, timeSlope);
}
int requiredSpdOLD = 0;
void DriveClass::rampUpdate(bool motSelect, int increment, bool optim)
{
    if (!rampInitDone[motSelect] || !increment) return;

    Timer_ramp[motSelect]->update();
    if (Timer_ramp[motSelect]->isStopped()) {
        Timer_ramp[motSelect]->start();
        State.requiredSpeed[motSelect] += increment;
        if (optim) {
            //Serial.println("rs: " + String(State.requiredSpeed[motSelect]) + " aENCs: " + String(State.actualEncSpeed[motSelect]));
            if (abs(State.requiredSpeed[motSelect]) > optimizedSpd.zeroPWM && abs(State.requiredSpeed[motSelect]) < 50 && State.actualEncSpeed[motSelect] == 0 && ((increment > 0 && State.requiredSpeed[motSelect] > 0)||(increment < 0 && State.requiredSpeed[motSelect] < 0)) && true) {
                Serial.println("Done IF");
                requiredSpdOLD = State.requiredSpeed[motSelect];
                while (!AccFromZero((!motSelect) ? sign(State.requiredSpeed[motSelect]) : 0, (motSelect) ? sign(State.requiredSpeed[motSelect]) : 0)) {
                    read();
                    Serial.println("Doing Acc");
                }
                State.requiredSpeed[motSelect] = requiredSpdOLD;
                rampInit(motSelect, SpeedRampDelayCalib);
            }
            Motors.SpeedSingle(motSelect, PWMtoOptimizedPWM(State.requiredSpeed[motSelect]));
        }
        else
            Motors.SpeedSingle(motSelect, State.requiredSpeed[motSelect]);
    }
}

void DriveClass::AccTillRotating_init(bool motSelect, int timeSlope)
{
    rampInit(motSelect, timeSlope, 0);
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
            rampInitDone[motSelect] = false;
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

int oldSpd[2] = { 0,0 };
bool DriveClass::DeccTillRotating_update(bool motSelect, unsigned int decrement, unsigned int endSpeed = 0)
{
    if (!deccTillRotatingDone[motSelect]) {
        if (State.actualEncSpeed[motSelect] > endSpeed) {
            rampUpdate(motSelect, -1*sign(State.actualSpeed[motSelect])*decrement);
            oldSpd[motSelect] = State.actualEncSpeed[motSelect];
        }
        else {
            State.motor2DeadBandPWM[motSelect] = State.actualSpeed[motSelect];
            State.motor2DeadBandReal[motSelect] = oldSpd[motSelect];
            rampInitDone[motSelect] = false;
            deccTillRotatingDone[motSelect] = true;
        }
    }
    return deccTillRotatingDone[motSelect];
}
void DriveClass::AccTillPWM_init(bool motSelect, int timeSlope)
{
    rampInit(motSelect, timeSlope);
    accTillPWMDone[motSelect] = false;
}
void DriveClass::AccTillPWM_init(bool motSelect, int timeSlope, int speedBegin, bool optim)
{
    //Motors.SpeedSingle(motSelect, speedBegin);
    rampInit(motSelect, timeSlope, speedBegin, optim);
    accTillPWMDone[motSelect] = false;
}

bool DriveClass::AccTillPWM_update(bool motSelect, int increment, int endSpeed, bool optim)
{
    if (!accTillPWMDone[motSelect]) {
        //Serial.println("Should end when " + String(State.actualSpeed[motSelect]) + ">=" + String(PWMtoOptimizedPWM(endSpeed)));
        if (State.actualSpeed[motSelect] < (optim ? PWMtoOptimizedPWM(endSpeed) : endSpeed)) //(optim ? PWMtoOptimizedPWM(State.actualSpeed[motSelect], 3, 20) : State.actualSpeed[motSelect])
            rampUpdate(motSelect, increment, optim);
        else {
            rampInitDone[motSelect] = false;
            accTillPWMDone[motSelect] = true;
        }
    }
    return accTillPWMDone[motSelect];
}

void DriveClass::DeccTillPWM_init(bool motSelect, int timeSlope, int speedBegin, bool optim)
{
    rampInit(motSelect, timeSlope, speedBegin, optim);
    deccTillPWMDone[motSelect] = false;
}

void DriveClass::DeccTillPWM_init(bool motSelect, int timeSlope)
{
    rampInit(motSelect, timeSlope);//, State.actualSpeed[motSelect]);
    deccTillPWMDone[motSelect] = false;
}

bool DriveClass::DeccTillPWM_update(bool motSelect, unsigned int decrement, int endSpeed, bool optim)
{
    if (!deccTillPWMDone[motSelect]) {
        if (State.actualSpeed[motSelect] > (optim ? PWMtoOptimizedPWM(endSpeed) : endSpeed))
            rampUpdate(motSelect, -decrement, optim);
        else {
            rampInitDone[motSelect] = false;
            deccTillPWMDone[motSelect] = true;
        }
    }
    return deccTillPWMDone[motSelect];
}

bool DriveClass::Meassure_deadband() {
    switch (state_deadbandMeas) {
    case State_deadbandMeas::Init: //Stop motors and reset some variables to zero
        Motors.Stop();
        State.requiredSpeed[0] = 0;
        State.requiredSpeed[1] = 0;
        State.CalibEnd = 0;
        state_deadbandMeas = State_deadbandMeas::Motor1_f;
        AccTillRotating_init(0, SpeedRampDelayCalib);
        AccTillRotating_init(1, SpeedRampDelayCalib);
        break;
    case State_deadbandMeas::Motor1_f: //Calibrate first motor - Increment PWM on motor until it starts rotating (the real speed from encoders is bigger than 1 rad/s)
    {
        bool ret = AccTillRotating_update(1, 1, 1);
        ret = AccTillRotating_update(0, 1, 1) && ret;
        if (ret) {
            //Serial.println("Speed of motor " + String(motSelect) + " after acceleration when start moving : [PWM] = " + String(State.motor1DeadBandPWM[motSelect]) + ", [ENC] = "+String(State.motor1DeadBandReal[motSelect]));
            state_deadbandMeas = State_deadbandMeas::Motor1_b;
        }
        break;
    }
    case State_deadbandMeas::Motor1_b:
        //DeccTillRotating_init(1, SpeedRampDelayCalib);
        //DeccTillRotating_init(0, SpeedRampDelayCalib);
        DeccTillPWM_init(0, SpeedRampDelayCalib);
        DeccTillPWM_init(1, SpeedRampDelayCalib);
        state_deadbandMeas = State_deadbandMeas::Motor2_f;
        break;
    case State_deadbandMeas::Motor2_f:
    {
        //bool ret2 = DeccTillRotating_update(0, 1, 1);
        //ret2 = DeccTillRotating_update(1, 1, 1) && ret2;

        bool ret2 = DeccTillPWM_update(0, 1, 0);
        ret2 = DeccTillPWM_update(1, 1, 0) && ret2;
        if (ret2) {
            //Serial.println("Speed of motor " + String(motSelect) + " after decceleration when stop moving : [PWM] = " + String(State.motor2DeadBandPWM[motSelect]) + ", [ENC] = " + String(State.motor2DeadBandReal[motSelect]));
            state_deadbandMeas = State_deadbandMeas::End;
        }
        break;
    }
    case State_deadbandMeas::End: //End of calibration of both motors - indicated by State.CalibEnd making true - After first calibration it stays in this state forever
        Motors.Stop();
        State.CalibEnd = 1;
        break;
    }
    return State.CalibEnd;
}

bool DriveClass::Meassure_linearityRamp(unsigned int maxSpeed, int motSelect, bool optim) {
    switch (state_rampMeas) {
    case State_rampMeas::InitAcc:
        AccTillPWM_init(motSelect, SpeedRampDelayCalib,-maxSpeed, optim);
        state_rampMeas = State_rampMeas::Acc;
        break;
    case State_rampMeas::Acc:
        if(AccTillPWM_update(motSelect, 1, maxSpeed, optim))
            state_rampMeas = State_rampMeas::InitDecc;
        break;
    case State_rampMeas::InitDecc:
        DeccTillPWM_init(motSelect, SpeedRampDelayCalib);
        state_rampMeas = State_rampMeas::Decc;
        break;
    case State_rampMeas::Decc:
        if(DeccTillPWM_update(motSelect, 1, 0, optim))
            state_rampMeas = State_rampMeas::End;
        break;
    case State_rampMeas::End:
        return true;
        break;
    }
    return false;
}

int speedDelay = 2;
bool ret = 1, ret2 = 1;
bool DriveClass::AccFromZero(int dir1, int dir2) {
    switch (state_accFromZero) {
    case State_accFromZero::Init:
        //first motor
        if (dir1 > 0)
            DeccTillPWM_init(0, speedDelay, 60);
        else if (dir1 < 0)
            AccTillPWM_init(0, speedDelay, -60);
        //second motor
        if (dir2 > 0)
            DeccTillPWM_init(1, speedDelay, 60);
        else if (dir2 < 0)
            AccTillPWM_init(1, speedDelay, -60);
        ret = 1, ret2 = 1; //in case this function got called with ret and ret2 not equal 1 - should not happen
        state_accFromZero = State_accFromZero::SlowDown;
        break;
    case State_accFromZero::SlowDown:
        if (dir1 > 0) {
            ret = DeccTillPWM_update(0, 2, 30);
        }
        else if(dir1 < 0)
        {
            ret = AccTillPWM_update(0, 2, -30);
        }
        if(dir2 > 0)
            ret2 = DeccTillPWM_update(1, 2, 30);
        else if(dir2 < 0)
            ret2 = AccTillPWM_update(1, 2, -30);

        if(ret && ret2)
            state_accFromZero = State_accFromZero::End;
        break;
    case State_accFromZero::End:
        accTillPWMDone[0] = false;
        accTillPWMDone[1] = false;
        deccTillPWMDone[0] = false;
        deccTillPWMDone[1] = false;
        state_accFromZero = State_accFromZero::Init;
        return true;
        break;
    }
    return false;
}

void DriveClass::loop()
{
    //Serial.println("$" + String(millis()) + " " + String(State.readSpeed) + ";");
    //Serial.print(printState_ramp());
    //Serial.println(" accDone: " + String(accTillPWMDone[State.meas.motSelect]) + " deccDone: " + String(deccTillPWMDone[State.meas.motSelect]));
    //{printState_comm} {State.requiredSpeed[0]} {printState_commPrev}
    //Serial.println("comm"+String(printState_comm)+ "CommPrev:"+String(printState_commPrev));
    read(); //Update speed of motors - reads encoders and writes speed of motors to State variables
    //Serial.println(printState_accFZ());
    //Read from State.state_comm and determine in which state controller wants driver to be in
    switch(State.state_comm){
    case State_comm::Stop:
        Motors.Stop();
        break;
    case State_comm::SpeedPWM: 
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    case State_comm::SpeedReal:
        if (!pid_In1 && !pid_Set1) {
            pid1.SetMode(MANUAL);
            pid_Out1 = 0;
        }
        else pid1.SetMode(AUTOMATIC);

        if (!pid_In2 && !pid_Set2) {
            pid2.SetMode(MANUAL);
            pid_Out2 = 0;
        }
        else pid2.SetMode(AUTOMATIC);

        Motors.Speed(PWMtoOptimizedPWM(roundf(pid_Out1)),
                     PWMtoOptimizedPWM(roundf(pid_Out2))); //PWMtoOptimizedPWM(
        break;
    case State_comm::Wait:
        state_deadbandMeas = State_deadbandMeas::Init;
        state_rampMeas = State_rampMeas::InitAcc;
        state_accFromZero = State_accFromZero::Init;
        break;
        //use linearized system
        //Motors.Speed(roundf(pid_Out1), roundf(pid_Out2)); //dont use linearized system
        //use pid to set real speed
    case State_comm::Meas:
        switch (State.meas.state_measType) {
        case State_measType::Calib:
            if(Meassure_deadband()) State.state_comm = State_comm::Wait;
            break;
        case State_measType::Ramp:
            if(Meassure_linearityRamp(PWMtoOptimizedPWM(40), State.meas.motSelect, 0)) State.state_comm = State_comm::Wait;
            //if (AccFromZero(-1,1)) State.state_comm = State_comm::Wait;
            break;
        case State_measType::Ramp_optim:
            if(Meassure_linearityRamp(40,State.meas.motSelect,1)) State.state_comm = State_comm::Wait; //maxSpd in non optimized PWM
            //Meassure_linearityRamp(50, State.meas.motSelect, 1);
            break;
        }
        break;
    case State_comm::CalibDeadBand:
        Meassure_deadband();
        break;
    default:
        break;
    }//{roundf(pid_Out1)} {commStatePrint}
}

DriveClass Drive{};

//Garbage:
    //Motors.SpeedSingle(motSelect, speedBegin);

//case State_comm::ChangeConstPID: //Change PID constants
//    pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1); //Constants should be already updated
//    State.state_comm = State.state_commPrev; //So it is here only once
//    break;

    //case State_deadbandMeas::Motor1_b:
    //    Serial.println("in 1_b");

    //    //Motors.Stop();

    //    //AccTillRotating_init(0, SpeedRampDelayCalib);
    //    //AccTillRotating_init(1, SpeedRampDelayCalib);

    //    //DeccTillRotating_init(0, SpeedRampDelayCalib);
    //    //DeccTillRotating_init(1, SpeedRampDelayCalib);
    //    state_deadbandMeas = State_deadbandMeas::Motor2_f;
    //    break;
    //case State_deadbandMeas::Motor2_f:
    //    Serial.println("in case");
    //    //state_deadbandMeas = State_deadbandMeas::End;
    //    //bool ret2 = DeccTillRotating_update(1, 1, 1);
    //    //ret2 = DeccTillRotating_update(0, 1, 0) && ret;
    //    //if (ret2)
    //    //    state_deadbandMeas = State_deadbandMeas::End;

    //    //bool ret2 = AccTillRotating_update(1, 1, 1);
    //    //ret2 = AccTillRotating_update(0, 1, 1) && ret2;
    //    //if (ret2)
    //    //    state_deadbandMeas = State_deadbandMeas::End;
    //    break;
