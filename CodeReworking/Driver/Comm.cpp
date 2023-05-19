/*****************************************************************//**
 * \file   Comm.cpp
 * \brief  Comm class source file
 * \details Used for I2C communication with the Controller, also setup Serial BaudRate
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#include "Comm.h"
#include "Drive.h"
 /**
  * \brief I2C send data - Wire request event
  */
void requestEvent()
{
    WireWriteI(State.actualSpeed[0]);
    WireWriteI(State.actualSpeed[1]);
    WireWriteI(State.actualEncSpeed[0]);
    WireWriteI(State.actualEncSpeed[1]);
    //WireWriteF(State.actualRealSpeed[0]);
    //WireWriteF(State.actualRealSpeed[1]);
}

/**
 * \brief I2C get data - Wire recieve event
 */
void receiveData(int x)
{
    if(State.state_commPrev != State.state_comm)
        State.state_commPrev = State.state_comm;
    int mode = Wire.read();
    //State.tempMode = mode;
    switch (mode) {
    case 0:
        //stop motor (and reset PID)
        State.requiredSpeed[0] = 0;
        State.requiredSpeed[1] = 0;
        State.state_comm = State_comm::Stop;
        break;
    case 1:
        State.state_comm = State_comm::Wait;
        break;
    case 2:
        //set speed
        State.requiredSpeed[0] = WireReadI();
        State.requiredSpeed[1] = WireReadI();
        State.state_comm = State_comm::SpeedPWM;
        break;
    case 3:
        State.requiredEncSpeed[0] = WireReadI();
        State.requiredEncSpeed[1] = WireReadI();
        State.state_comm = State_comm::SpeedReal;
        break;
    case 4:
        State.Kp_1 += WireReadF();
        State.Ki_1 += WireReadF();
        State.Kd_1 += WireReadF();
        State.Kp_2 = State.Kp_1;
        State.Ki_2 = State.Ki_1;
        State.Kd_2 = State.Kd_2;
        Drive.pid1.SetTunings(State.Kp_1, State.Ki_1, State.Kd_1);
        Drive.pid2.SetTunings(State.Kp_2, State.Ki_2, State.Kd_2);
        //State.state_comm = State_comm::ChangeConstPID;
        break;
    case 5:
        State.state_comm = State_comm::CalibDeadBand;
        break;
    case 10:
        State.meas.motSelect = WireReadI();
        State.meas.temp = WireReadI();
        State.meas.state_measType = static_cast<State_measType>(State.meas.temp);
        State.state_comm = State_comm::Meas;
        break;
    default:
        State.state_comm = State_comm::Unknown;
        break;
    }
}
void CommClass::init()
{
    Serial.begin(BaudRate);
    Wire.begin(State.address);
    Wire.onReceive(receiveData);
    Wire.onRequest(requestEvent);
    Serial.println();
}

CommClass Comm{};

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
    //    /*
    // ("motor speed1: ");Serial.print(motorspeed1); Serial.print(" ");
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
//}
//void CommClass::init()
//{
//    Serial.begin(BaudRate);
//    Wire.begin(State.address);
//    Wire.onRequest(requestEvent);
//    Wire.onReceive(receiveData);
//}
