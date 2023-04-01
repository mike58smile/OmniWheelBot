/*****************************************************************//**
 * \file   Comm.cpp
 * \brief  Comm class source file
 * \details Used for I2C communication with the Controller
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Comm.h"

void CommClass::init()
{
	Serial.begin(BaudRate);
	Wire.begin(State.address);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveData);
}


void requestEvent()
{

}

void receiveData(int x)
{ 
    int mode = Wire.read();
    switch(mode) {
    case 0:
        //stop motor and reset PID
        State.commState = Stop;
        break;
    case 1:
        //set speed
        State.requiredSpeed[0] = WireRead;
        State.requiredSpeed[1] = WireRead;
        State.commState = Speed;
        break;
    default:
        break;
    }
    //if (mode == 0) { //funkcia ResetSetpoint v Main arduine
    //    syn_rozhodovac = false;
    //    rozhodovac2 = false;
    //    stopmotor();
    //    motorcontrol1 = 3;
    //    motorcontrol2 = 3;
    //    Setpoint = 0;
    //    enc1.write(0);
    //    enc2.write(0);
    //    position1, position2, setpoint2, setpoint1 = 0;
    //}

    //else if (mode == 1) { //funkcia SetSpeed v Main arduine
    //    syn_rozhodovac = false;
    //    rozhodovac2 = false;
    //    motorcontrol1 = 1;
    //    motorcontrol2 = 1;
    //    motorspeed1 = (Wire.read() | Wire.read() << 8);
    //    motorspeed2 = (Wire.read() | Wire.read() << 8);
    //}
    //else if (mode == 10) {
    //    syn_rozhodovac = true;
    //    rozhodovac2 = false;
    //    motorspeed1_synchro = (Wire.read() | Wire.read() << 8);
    //    motorspeed2_synchro = (Wire.read() | Wire.read() << 8);
    //}
    //else if (mode == 8) {
    //    /*Serial.print("motor speed1: ");Serial.print(motorspeed1); Serial.print(" ");
    //    Serial.print("kraj 1: ");Serial.println(kraj_sp1);*/
    //    syn_rozhodovac = false;
    //    rozhodovac2 = true;
    //    motorspeed1 = (Wire.read() | Wire.read() << 8);
    //    kraj_sp1 = (Wire.read() | Wire.read() << 8);
    //    motorspeed2 = (Wire.read() | Wire.read() << 8);
    //    kraj_sp2 = (Wire.read() | Wire.read() << 8);
    //}
    //else if (mode == 16) {
    //    Kp_sp += (double)(Wire.read() | Wire.read() << 8) / 10;
    //    Kd_sp += (double)(Wire.read() | Wire.read() << 8) / 1000;
    //    Ki_sp += (double)(Wire.read() | Wire.read() << 8) / 10;
    //    const_speed1.SetTunings(Kp_sp, Ki_sp, Kd_sp);
    //    const_speed2.SetTunings(Kp_sp, Ki_sp, Kd_sp);
    //}
    //else { //funkcia Setsetpoint v Main arduine
    //    syn_rozhodovac = false;
    //    rozhodovac2 = false;
    //    motorcontrol1 = 0;
    //    motorcontrol2 = 0;
    //    setpoint1 = (Wire.read() | Wire.read() << 8);
    //    setpoint2 = (Wire.read() | Wire.read() << 8);
    //}
}


CommClass Comm;

