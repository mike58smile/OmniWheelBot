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
        //Serial.println(currentTime - previousTime);
        State.actualEncSpeed[0] = enc1.readAndReset();
        State.actualEncSpeed[1] = enc2.readAndReset();
        State.readSpeed = currentTime - previousTime; //Save reading speed
       // State.actualRealSpeed[0] = EncToRealSpd(State.actualEncSpeed[0]);
        //State.actualRealSpeed[1] = EncToRealSpd(State.actualEncSpeed[1]);
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
    Motors.SpeedSingle(motSelect, (optim ? PWMtoOptimizedPWM(State.requiredSpeed[motSelect]) : State.requiredSpeed[motSelect]));
    rampInit(motSelect, timeSlope);
}

void DriveClass::rampUpdate(bool motSelect, int increment, bool optim)
{
    if (!rampInitDone[motSelect] || !increment) return;

    Timer_ramp[motSelect]->update();
    if (Timer_ramp[motSelect]->isStopped()) {
        Timer_ramp[motSelect]->start();
        State.requiredSpeed[motSelect] += increment;
        if(optim)
            Motors.SpeedSingle(motSelect, PWMtoOptimizedPWM(State.requiredSpeed[motSelect]));
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
            Serial.println("Speed of motor " + String(motSelect) + " after acceleration when start moving : [PWM] = " + String(State.motor1DeadBandPWM[motSelect]) + ", [ENC] = "+String(State.motor1DeadBandReal[motSelect]));
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

    //Motors.SpeedSingle(motSelect, speedBegin);
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
        if (ret)
            state_deadbandMeas = State_deadbandMeas::Motor1_b;
        break;
    }
    case State_deadbandMeas::Motor1_b:
        DeccTillRotating_init(0, SpeedRampDelayCalib);
        DeccTillRotating_init(1, SpeedRampDelayCalib);
        state_deadbandMeas = State_deadbandMeas::Motor2_f;
        break;
    case State_deadbandMeas::Motor2_f:
    {
        bool ret2 = DeccTillRotating_update(0, 1, 1);
        ret2 = DeccTillRotating_update(1, 1, 1) && ret2;
        if (ret2)
            state_deadbandMeas = State_deadbandMeas::End;
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

void DriveClass::loop()
{
    //{printState_comm} {State.requiredSpeed[0]} {printState_commPrev}
    //Serial.println("comm"+String(printState_comm)+ "CommPrev:"+String(printState_commPrev));
    read(); //Update speed of motors - reads encoders and writes speed of motors to State variables
    
    //Read from State.state_comm and determine in which state controller wants driver to be in
    switch(State.state_comm){
    case State_comm::Stop:
        Motors.Stop();
        //enc1.write(0); //odstranit probably
        //enc2.write(0);//odstranit probably
        break;
    case State_comm::Wait:
        state_deadbandMeas = State_deadbandMeas::Init;
        state_rampMeas = State_rampMeas::InitAcc;
        break;
    case State_comm::SpeedPWM: 
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    case State_comm::SpeedReal:
        //pid_In1 = State.actualRealSpeed[0];
        //pid_Set1 = State.requiredRealSpeed[0];
        //pid1.Compute();
        //if (abs(roundf(pid_Out1)) < (State.motor1DeadBand[0] - 10))
        //    pid_Out1 = sign(pid_Out1) * (State.motor1DeadBand[0] - 10);
        // 
        
        //if (abs(roundf(pid_Out1)) <= 5)
        //    actualPidOut1 = 0;
        //else
        //    actualPidOut1 = sign(roundf(pid_Out1))*(abs(roundf(pid_Out1)) + deadbandPWM);
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
        Motors.Speed(PWMtoOptimizedPWM(roundf(pid_Out1)), PWMtoOptimizedPWM(roundf(pid_Out2)));
        //Motors.Speed(roundf(pid_Out1), 0);
        //use pid to set real speed
        break;
    case State_comm::Meas:
        switch (State.meas.state_measType) {
        case State_measType::Calib:
            if(Meassure_deadband()) State.state_comm = State_comm::Wait;
            break;
        case State_measType::Ramp:
            if(Meassure_linearityRamp(70, State.meas.motSelect, 0)) State.state_comm = State_comm::Wait;
            //Meassure_linearityRamp(70, State.meas.motSelect, 0);
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
    case State_comm::Meas1:
        Meassure_linearityRamp(50, State.meas.motSelect, 1);
        break;
    //case State_comm::ChangeConstPID: //Change PID constants
    //    pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1); //Constants should be already updated
    //    State.state_comm = State.state_commPrev; //So it is here only once
    //    break;
    default:
        break;
    }//{roundf(pid_Out1)} {commStatePrint}
}

DriveClass Drive{};

//Garbage:
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
