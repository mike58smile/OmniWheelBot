/*****************************************************************//**
 * \file   Movements.cpp
 * \brief  Movements class source file
 * \details 
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/
#include "Movements.h"

void MovementsClass::init()
{


}

void MovementsClass::circle(float spd, float radius)
{
    //pohyb po kruznici
        CurrentTime = millis();
        if ((unsigned long)(CurrentTime - ElapsedTime) >= TimerSpeedDelay_mS) {
            ElapsedTime = CurrentTime;
            tempAlfa += 57.295779513082320876798154814105 * (spd * TimerSpeedDelay_S / radius);
        }
    calcSpd(State.wantedV, tempAlfa, 0);
}

void MovementsClass::calcSpd(float spd, int alfa, float w)
{
    bool invert = 0;
    alfa = fmod(alfa, 360);
    if ((alfa <= -90 && alfa > -270) || (alfa >= 90 && alfa < 270)) invert = 1; //kvoli nespojitostiam tangensu
    alfaR = alfa * DEG_TO_RAD;
    alfaR = fmod(alfaR, TWO_PI);
    float tg_alfaR = tan(alfaR);
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
    float t = millis() / 1000.0; // get program time
    //old print Serial.println("alfa: "+String(alfa)+" "+"spd1:"+String(spd1)+"spd2:"+String(spd2)+"spd3:"+String(spd3)+"spd4:"+String(spd4)+"w1:"+String(w1)+"w2:"+String(w2)+"w3:"+String(w3)+"w4:"+String(w4)+"vy:"+String(vy)+"vx"+String(vx));//+"tg_alfaR:"+String(tg_alfaR)+"pow(tg_alfaR,2) + 1:"+String(pow(tg_alfaR,2) + 1)+"sqrt(pow(tg_alfaR,2) + 1):"+String(sqrt(pow(tg_alfaR,2) + 1))+"sin(alfa3):"+String(sin(alfa3))
    //Serial.println(String(alfaR) + " " + String(spd1) + " " + String(spd2) + " " + String(spd3) + " " + String(spd4) + " " + String(w1) + " " + String(w2) + " " + String(w3) + " " + String(w4) + " " + String(vy) + " " + String(vx));
    Comm.SetReal(w1, w2, w3, w4);
}

void MovementsClass::loop()
{
    switch (State.state_movement) {
    case State_movement::IR_movement:
        break;
    case State_movement::CalcSpd:
        calcSpd(State.wantedV, State.wantedAlfa, State.wantedW);
        break;
    case State_movement::Circle:
        circle(State.wantedV, State.wantedRadius);
        break;
    }
}


MovementsClass Movements;

