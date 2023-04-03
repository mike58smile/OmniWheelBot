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



//DriveClass Drive; //also calls right c'tors to setup Encoder objects

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


