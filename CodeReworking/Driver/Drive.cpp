/*****************************************************************//**
 * \file   Drive.cpp
 * \brief  Drive class source file, also creates an object called Drive
 * \details Here are all movements -> it control motors
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

//#include "Motors.h"
#include "Drive.h"

//Global variables
volatile bool flagReadSpeed = 0; ///< Will be set true in interrupt routine
volatile int interruptNum = 0; ///< Will increment in interrupt routine


void DriveClass::init()
{
    Motors.init();
}
void DriveClass::loop()
{
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
        //use pid to set real speed
        break;
    default:
        break;
    }
}


