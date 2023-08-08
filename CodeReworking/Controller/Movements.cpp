/*****************************************************************//**
 * \file   Movements.cpp
 * \brief  Movements class source file
 * \details Class implementing kinematics calculations and advanced movements
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/
#include "Movements.h"

void MovementsClass::init()
{
    pid.SetMode(AUTOMATIC);
    pid.SetOutputLimits(-20,20); //Set max output of PID action
    Gyro.init();
    Serial.println("DONE");
}

void MovementsClass::circle(float spd, float radius, float w)
{
    //pohyb po kruznici
        CurrentTime = millis();
        if ((unsigned long)(CurrentTime - ElapsedTime) >= TimerSpeedDelay_mS) {
            ElapsedTime = CurrentTime;
            tempAlfa += 57.295779513082320876798154814105 * (spd * TimerSpeedDelay_S / radius);
        }
    calcSpd(State.wantedV, tempAlfa, w);
}

void MovementsClass::calcSpd(float spd, int alfa, float w)
{
    bool invert = 0;
    alfa = fmod(alfa, 360);
    if ((alfa <= -90 && alfa > -270) || (alfa >= 90 && alfa < 270)) invert = 1; //kvoli nespojitostiam tangensu
    alfaR = alfa * DEG_TO_RAD;
    alfaR = fmod(alfaR, TWO_PI);
    tg_alfaR = tan(alfaR);
    vx = spd / (sqrt(pow(tg_alfaR, 2) + 1));
    vy = spd * tg_alfaR / (sqrt(pow(tg_alfaR, 2) + 1));
    if (invert) {
        (vx != 0) ? vx *= -1 : vx = 0;
        (vy != 0) ? vy *= -1 : vy = 0;
    }
    w1 = (B * w + vy * cos(alfa1) - vx * sin(alfa1)) / R;
    w2 = (B * w + vy * cos(alfa2) - vx * sin(alfa2)) / R;
    w3 = (B * w + vy * cos(alfa3) - vx * sin(alfa3)) / R;
    w4 = (B * w + vy) / R;
    Comm.SetReal(w1, w2, w3, w4);
}

void MovementsClass::loop()
{
    switch (State.state_movement) {
    case State_movement::IR_movement:
        break;
    case State_movement::CalcSpd:
        gyroPid();
        Serial.println("$" + String(millis()) + " " + String(State.wantedW) + " " + String(pid_In, 2) + " " + String(pid_Out) + " " + String(State.Kp) + " " + String(State.Ki) + " " + String(State.Kd) + " " + String(State.gyroUpdateMS) + ";"); //+ ((pid_In > 0) ? " " : "") //pred PID in
        State.GyroRegON ? calcSpd(State.wantedV, State.wantedAlfa, pid_Out) : calcSpd(State.wantedV, State.wantedAlfa, State.wantedW);
        break;
    case State_movement::Circle:
        gyroPid();
        Serial.println("$" + String(millis()) + " " + String(State.wantedW) + " " + String(pid_In, 2) + " " + String(pid_Out) + " " + String(State.Kp) + " " + String(State.Ki) + " " + String(State.Kd) + " " + String(State.gyroUpdateMS) + ";"); //+ ((pid_In > 0) ? " " : "") //pred PID in
        State.GyroRegON ? circle(State.wantedV, State.wantedRadius, pid_Out) : circle(State.wantedV, State.wantedRadius, State.wantedW);
        break;
    case State_movement::MeasGyro:
        calcSpd(0, 0, State.wantedW);
        break;
    case State_movement::PidGyro:
        if ((pid.GetKp() != State.Kp) || (pid.GetKi() != State.Ki) || (pid.GetKd() != State.Kd))
            pid.SetTunings(State.Kp, State.Ki, State.Kd);
        gyroPid();
        calcSpd(0, 0, pid_Out);
        Serial.println("$" + String(millis()) + " " + String(State.wantedW) + " " + String(pid_In, 2) + " " + String(pid_Out) + " " + String(State.Kp) + " " + String(State.Ki) + " " + String(State.Kd) + " " + String(State.gyroUpdateMS) + ";"); //+ ((pid_In > 0) ? " " : "") //pred PID in
        break;

    case State_movement::ShowTime_Dir:
    {
        gyroPid();
        //Serial.println("$" + String(millis()) + " " + String(State.wantedW) + " " + String(pid_In, 2) + " " + String(pid_Out) + " " + String(State.Kp) + " " + String(State.Ki) + " " + String(State.Kd) + " " + String(State.gyroUpdateMS) + ";"); //+ ((pid_In > 0) ? " " : "") //pred PID in
        State.GyroRegON ? calcSpd(State.wantedV, State.wantedAlfa, pid_Out) : calcSpd(State.wantedV, State.wantedAlfa, State.wantedW);
        //calcSpd(State.wantedV, State.wantedAlfa, pid_Out);
        break;
    }
    case State_movement::ShowTime_Circ:
    {
        //Serial.println(State.stopCirc);
        if (!State.stopCirc) {
            gyroPid();
            //Serial.print(State.GyroRegON);
            //Serial.print(" ");
            //Serial.println(State.stopCirc);
            //Serial.print(" ");
            //Serial.println("$" + String(millis()) + " " + String(State.wantedW) + " " + String(pid_In, 2) + " " + String(pid_Out) + " " + String(State.Kp) + " " + String(State.Ki) + " " + String(State.Kd) + " " + String(State.gyroUpdateMS) + ";"); //+ ((pid_In > 0) ? " " : "") //pred PID in
            State.GyroRegON ? circle(State.wantedV, State.wantedRadius, pid_Out) : circle(State.wantedV, State.wantedRadius, State.wantedW);
            //circle(State.wantedV, State.wantedRadius, pid_Out);
        }
        else {
            tempAlfa = 0;
            calcSpd(0, 0, 0);
        }
        break;
    }
    }
}

void MovementsClass::gyroTest()
{
    //if (millis() - timer > State.gyroUpdateMS) {

    //}
    gyro.read();
    //Serial.println("$" + String(gyro.read(),2) + ";");
    //timer = millis();
}

void MovementsClass::gyroPid()
{
    //mpu6050.update();
    if (millis() - timer > State.gyroUpdateMS) {
        //long angleVelocityZ = long(mpu6050.getGyroZ() * 1000.0); //for calculations with long - removes float
        pid_In = gyro.read();
        pid_Set = State.wantedW;
        if (!pid_Set && !pid_In) {
            pid.SetMode(MANUAL);
            pid_Out = 0;
        }
        else pid.SetMode(AUTOMATIC);

        if (fabs(pid_Set - pid_In) > 0.1)
            pid.Compute();
        timer = millis();
    }
}


MovementsClass Movements{};

//Garbage:
//old print Serial.println("alfa: "+String(alfa)+" "+"spd1:"+String(spd1)+"spd2:"+String(spd2)+"spd3:"+String(spd3)+"spd4:"+String(spd4)+"w1:"+String(w1)+"w2:"+String(w2)+"w3:"+String(w3)+"w4:"+String(w4)+"vy:"+String(vy)+"vx"+String(vx));//+"tg_alfaR:"+String(tg_alfaR)+"pow(tg_alfaR,2) + 1:"+String(pow(tg_alfaR,2) + 1)+"sqrt(pow(tg_alfaR,2) + 1):"+String(sqrt(pow(tg_alfaR,2) + 1))+"sin(alfa3):"+String(sin(alfa3))
//Serial.println(String(alfaR) + " " + String(spd1) + " " + String(spd2) + " " + String(spd3) + " " + String(spd4) + " " + String(w1) + " " + String(w2) + " " + String(w3) + " " + String(w4) + " " + String(vy) + " " + String(vx));

