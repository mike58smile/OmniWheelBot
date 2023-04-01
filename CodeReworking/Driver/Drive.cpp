// 
// 
// 

#include "Drive.h"

void DriveClass::init()
{
    Motors.init();
}

void DriveClass::loop()
{
    switch(State.commState){
    case Stop:
        Motors.Stop();
        break;
    case Speed:
        Motors.Speed(State.requiredSpeed[0], State.requiredSpeed[1]);
        break;
    default:
        break;
    }
}

DriveClass Drive;

