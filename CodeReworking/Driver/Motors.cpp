/*****************************************************************//**
 * \file   Motors.cpp
 * \brief  Motors.cpp class header
 * \details 
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

}

void MotorsClass::Stop()
{
    analogWrite(State.M1_RPWM, 0);
    analogWrite(State.M1_LPWM, 0);
    analogWrite(State.M2_RPWM, 0);
    analogWrite(State.M2_LPWM, 0);
}

void MotorsClass::Speed(int Spd1, int Spd2)
{
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

}