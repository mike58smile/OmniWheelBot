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
}

/**
 * \brief Read data from drivers and writes them to State variables
 */
void CommClass::loop()
{
    Wire.requestFrom(State.adress[0], countBytes(2,2));
	State.actualSpeed[0] = WireReadI();
	State.actualSpeed[1] = WireReadI();
    State.actualRealSpeed[0] = WireReadF();
	State.actualRealSpeed[1] = WireReadF();

    Wire.requestFrom(State.adress[1], countBytes(2, 2));
    State.actualSpeed[2] = WireReadI();
    State.actualSpeed[3] = WireReadI();
    State.actualRealSpeed[2] = WireReadF();
    State.actualRealSpeed[3] = WireReadF();
}

void CommClass::stop()
{
	Wire.beginTransmission(State.adress[0]);
	Wire.write(0);
	Wire.endTransmission();
	Wire.beginTransmission(State.adress[1]);
	Wire.write(0);
	Wire.endTransmission();
	State.controlState = ControlState::Stop;

}

void CommClass::SetPWM(int spd1, int spd2, int spd3, int spd4)
{
	if (!spd1 && !spd2 && !spd3 && !spd4)
		return stop();

	Wire.beginTransmission(State.adress[0]);
	Wire.write(2);
	WireWriteI(spd1);
	WireWriteI(spd2);
	Wire.endTransmission();

	Wire.beginTransmission(State.adress[1]);
	Wire.write(2);
	WireWriteI(spd3);
	WireWriteI(spd4);
	Wire.endTransmission();
	State.controlState = ControlState::SpeedPWM;

}
void CommClass::SerialDebug() {
    if (Serial.available() > 0) {
        SerialString = Serial.readStringUntil('\n');
        SerialInt = SerialString.toInt();
        if (SerialString == "com" || SerialString == "comm")
            EnableSerialMode = 'c';
        else if (SerialString == "spd" || SerialString == "speed")
            EnableSerialMode = 's';
        switch (EnableSerialMode) {
        case 'c':
            switch (SerialInt) {
            case 0:
                //stop motor (and reset PID)
                stop();
                break;
            case 1:
                State.controlState = ControlState::Wait;
                break;
            case 2:
                //set speed
                SetPWM(50, 50, 50, 50);
                break;
            default:
                break;
            }
            break;
        case 's':
            int speed = SerialInt;
            if (abs(speed) > 200) speed = sign(speed) * 200;
            SetPWM(speed, speed, speed, speed);
            break;
        default:
            break;
        }
    }
}
CommClass Comm(State);