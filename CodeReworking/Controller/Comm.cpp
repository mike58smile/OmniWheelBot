/*****************************************************************//**
 * \file   Comm.cpp
 * \brief  Comm class source file
 * \details Used for I2C communication with the, also setup Serial BaudRate
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#include "Comm.h"
#include <i2cdetect.h>

/**
* \brief Initialize Serial and I2C communication
* \note Use in void setup()
*/
void CommClass::init()
{
	Serial.begin(BaudRate);
	Wire.begin();
    Serial.println("START Program " __FILE__ " from " __DATE__);
}

/**
 * \brief Read data from drivers and writes them to State variables
 */
void CommClass::loop()
{
    Wire.requestFrom(State.adress[0], countBytes(4, 0)); //Need to change countBytes, maybe use macro __SIZEOF_FLOAT__ and also for int
	State.actualSpeed[0] = WireReadI();
	State.actualSpeed[1] = WireReadI();
    State.actualEncSpeed[0] = WireReadI();
    State.actualEncSpeed[1] = WireReadI();
    //State.actualRealSpeed[0] = WireReadF();
	//State.actualRealSpeed[1] = WireReadF();

    Wire.requestFrom(State.adress[1], countBytes(4, 0)); //Need to change countBytes
    State.actualSpeed[3] = WireReadI();
    State.actualSpeed[2] = WireReadI();
    State.actualEncSpeed[3] = WireReadI();
    State.actualEncSpeed[2] = WireReadI();
    //State.actualRealSpeed[2] = WireReadF();
    //State.actualRealSpeed[3] = WireReadF();
}

void CommClass::Stop()
{
	Wire.beginTransmission(State.adress[0]);
	Wire.write(0); //Send only 8bit for state recognition
	Wire.endTransmission();
	Wire.beginTransmission(State.adress[1]);
	Wire.write(0);
	Wire.endTransmission();
	State.controlState = ControlState::Stop;
}

void CommClass::SetPWM(int spd1, int spd2, int spd3, int spd4)
{
	if (!spd1 && !spd2 && !spd3 && !spd4)
		return Stop();

	Wire.beginTransmission(State.adress[0]);
	Wire.write(2);
	WireWriteI(spd1);
	WireWriteI(spd2);
	Wire.endTransmission();

	Wire.beginTransmission(State.adress[1]);
	Wire.write(2);
	WireWriteI(spd4);
	WireWriteI(spd3);
	Wire.endTransmission();
	State.controlState = ControlState::SpeedPWM;
}

void CommClass::SetPWM(int spd)
{
    if (!spd) //spd == 0
        return Stop();
    SetPWM(spd, spd, spd, spd);
}

void CommClass::SetReal(float spd1, float spd2, float spd3, float spd4)
{
    Wire.beginTransmission(State.adress[0]);
    Wire.write(3);
    WireWriteI(RealToEncSpd(spd1));
    WireWriteI(RealToEncSpd(spd2));
    Wire.endTransmission();

    Wire.beginTransmission(State.adress[1]);
    Wire.write(3);
    WireWriteI(RealToEncSpd(spd4)); //Different then above
    WireWriteI(RealToEncSpd(spd3));
    Wire.endTransmission();
    State.controlState = ControlState::SpeedReal;
}

void CommClass::SetRealEnc(int spd1, int spd2, int spd3, int spd4)
{
    State.requiredEncSpeed[0] = spd1;
    State.requiredEncSpeed[1] = spd2;
    State.requiredEncSpeed[2] = spd3;
    State.requiredEncSpeed[3] = spd4;

    Wire.beginTransmission(State.adress[0]);
    Wire.write(3);
    WireWriteI(spd1);
    WireWriteI(spd2);
    Wire.endTransmission();

    Wire.beginTransmission(State.adress[1]);
    Wire.write(3);
    WireWriteI(spd4);
    WireWriteI(spd3);
    Wire.endTransmission();
    State.controlState = ControlState::SpeedReal;
}


void CommClass::SetPID(float Kp_add, float Ki_add, float Kd_add)
{
    State.Kp_1 += Kp_add; State.Ki_1 += Ki_add; State.Kd_1 += Kd_add;
    Wire.beginTransmission(State.adress[0]);
    Wire.write(4);
    WireWriteF(Kp_add);
    WireWriteF(Ki_add);
    WireWriteF(Kd_add);
    Wire.endTransmission();
    //constants for the next motor same... useless
    Wire.beginTransmission(State.adress[1]);
    Wire.write(4);
    WireWriteF(Kp_add);
    WireWriteF(Ki_add);
    WireWriteF(Kd_add);
    Wire.endTransmission();

    State.controlState = ControlState::SetPID;
}

void CommClass::SetMeas(MeasType type, int motSelect)
{
    if (motSelect == 0 || motSelect == 1) {
        Wire.beginTransmission(State.adress[0]);
        Wire.write(10);
        WireWriteI(motSelect);
        WireWriteI(static_cast<int>(type));
        Wire.endTransmission();
    }
    else if (motSelect == 2 || motSelect == 3) {
        Wire.beginTransmission(State.adress[1]);
        Wire.write(10);
        WireWriteI(motSelect - 2);
        WireWriteI(static_cast<int>(type));
        Wire.endTransmission();
    }
    State.controlState = ControlState::SetMeas;
}

CommClass Comm(State);

//Garbage:
// In Comm.h
// 	 void SerialDebug();
// In Comm.cpp
//void CommClass::SerialDebug() {
//    if (Serial.available() > 0) {
//        SerialString = Serial.readStringUntil('\n');
//        SerialInt = SerialString.toInt();
//        if (SerialString == "com" || SerialString == "comm")
//            EnableSerialMode = 'c';
//        else if (SerialString == "spd" || SerialString == "speed")
//            EnableSerialMode = 's';
//        switch (EnableSerialMode) {
//        case 'c':
//            switch (SerialInt) {
//            case 0:
//                //Stop motor (and reset PID)
//                Stop();
//                break;
//            case 1:
//                State.controlState = ControlState::Wait;
//                break;
//            case 2:
//                //set speed
//                SetPWM(50, 50, 50, 50);
//                break;
//            default:
//                break;
//            }
//            break;
//        case 's':
//            int speed = SerialInt;
//            if (abs(speed) > 200) speed = sign(speed) * 200;
//            SetPWM(speed, speed, speed, speed);
//            break;
//        default:
//            break;
//        }
//    }
//}
