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

    //Motor Driver setup -> Puts High on motor inputs
    digitalWrite(State.M1_REN, HIGH);
    digitalWrite(State.M1_LEN, HIGH);
    digitalWrite(State.M2_REN, HIGH);
    digitalWrite(State.M2_LEN, HIGH);

    State.actualState = MainState::Setup; //Define in which state the driver operates
}

void MotorsClass::Stop()
{
    analogWrite(State.M1_RPWM, 0);
    analogWrite(State.M1_LPWM, 0);
    analogWrite(State.M2_RPWM, 0);
    analogWrite(State.M2_LPWM, 0);
    State.actualState = MainState::Stop; //Define in which state the driver operates
}

void MotorsClass::Speed(int Spd1, int Spd2)
{
    if (!Spd1 && !Spd2) {
        Stop();
        return;
    }
    // Set speed of first motor
    if (Spd1 >= 0) {
        analogWrite(State.M1_RPWM, abs(Spd1));
        analogWrite(State.M1_LPWM, 0);
    }
    else{
        analogWrite(State.M1_RPWM, 0);
        analogWrite(State.M1_LPWM, abs(Spd1));
    }

    // Set speed of second motor
    if (Spd2 >= 0) {
        analogWrite(State.M2_RPWM, abs(Spd2));
        analogWrite(State.M2_LPWM, 0);
    }
    else {
        analogWrite(State.M2_RPWM, 0);
        analogWrite(State.M2_LPWM, abs(Spd2));
    }
    
    // Save current motor speeds to State
    State.actualSpeed[0] = Spd1;
    State.actualSpeed[1] = Spd2;

    State.actualState = MainState::Speed; //Define in which state the driver operates
}