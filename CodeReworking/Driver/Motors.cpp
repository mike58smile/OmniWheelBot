/*****************************************************************//**
 * \file   Motors.cpp
 * \brief  Motors class source file
 * \details Used for direct motor control
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Motors.h"

void MotorsClass::init()
{
    //Define pinMode for Motor 1
    pinMode(State.M1_RPWM, OUTPUT);
    pinMode(State.M1_LPWM, OUTPUT);
    pinMode(State.M1_REN, OUTPUT);
    pinMode(State.M1_LEN, OUTPUT);

    //Define pinMode for Motor 2
    pinMode(State.M2_RPWM, OUTPUT);
    pinMode(State.M2_LPWM, OUTPUT);
    pinMode(State.M2_REN, OUTPUT);
    pinMode(State.M2_LEN, OUTPUT);

    //Motor Driver setup -> Puts High on 
    digitalWrite(State.M1_REN, HIGH);
    digitalWrite(State.M1_LEN, HIGH);
    digitalWrite(State.M2_REN, HIGH);
    digitalWrite(State.M2_LEN, HIGH);

    State.state_actual = State_actual::Setup; //Define in which state the driver operates
}

void MotorsClass::Stop()
{
    analogWrite(State.M1_RPWM, 0);
    analogWrite(State.M1_LPWM, 0);
    analogWrite(State.M2_RPWM, 0);
    analogWrite(State.M2_LPWM, 0);

    // Save current motor speeds to State
    State.actualSpeed[0] = 0;
    State.actualSpeed[1] = 0;
    State.state_actual = State_actual::Stop; //Define in which state the driver operates
}

void MotorsClass::SpeedSingle(bool motSelect, int Spd)
{
    if (abs(Spd) > State.maxSpeed)
        Spd = sign(Spd) * State.maxSpeed;
    if (motSelect == 0) {
        // Set speed of first motor
        if (Spd <= 0) {
            analogWrite(State.M1_LPWM, 0);
            analogWrite(State.M1_RPWM, abs(Spd));
        }
        else {
            analogWrite(State.M1_RPWM, 0);
            analogWrite(State.M1_LPWM, abs(Spd));
        }
        // Save current motor speeds to State
        State.actualSpeed[0] = Spd;

        State.state_actual = State_actual::Speed; //Define in which state the driver operates
    }
    else {
        // Set speed of second motor
        if (Spd <= 0) {
            analogWrite(State.M2_LPWM, 0);
            analogWrite(State.M2_RPWM, abs(Spd));
        }
        else {
            analogWrite(State.M2_RPWM, 0);
            analogWrite(State.M2_LPWM, abs(Spd));
        }
        // Save current motor speeds to State
        State.actualSpeed[1] = Spd;

        State.state_actual = State_actual::Speed; //Define in which state the driver operates
    }
}


void MotorsClass::Speed(int Spd1, int Spd2)
{
    if (!Spd1 && !Spd2) {
        Stop();
        return;
    }
    SpeedSingle(0, Spd1);
    SpeedSingle(1, Spd2);
    State.state_actual = State_actual::Speed; //Define in which state the driver operates
}


//void MotorsClass::Speed_1(int Spd)
//{
//    // Set speed of first motor
//    if (Spd <= 0) {
//        analogWrite(State.M1_LPWM, 0);
//        analogWrite(State.M1_RPWM, abs(Spd));
//    }
//    else {
//        analogWrite(State.M1_RPWM, 0);
//        analogWrite(State.M1_LPWM, abs(Spd));
//    }
//    // Save current motor speeds to State
//    State.actualSpeed[0] = Spd;
//
//    State.state_actual = State_actual::Speed; //Define in which state the driver operates
//}
//
//void MotorsClass::Speed_2(int Spd)
//{
//    // Set speed of second motor
//    if (Spd <= 0) {
//        analogWrite(State.M2_LPWM, 0);
//        analogWrite(State.M2_RPWM, abs(Spd));
//    }
//    else {
//        analogWrite(State.M2_RPWM, 0);
//        analogWrite(State.M2_LPWM, abs(Spd));
//    }
//    // Save current motor speeds to State
//    State.actualSpeed[1] = Spd;
//
//    State.state_actual = State_actual::Speed; //Define in which state the driver operates
//}