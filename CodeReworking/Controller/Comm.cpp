/*****************************************************************//**
 * \file   Comm.cpp
 * \brief  Comm class source file
 * \details 
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#include "Comm.h"
#include <Wire.h>
#include <i2cdetect.h>
void CommClass::init()
{
	Serial.begin(BaudRate);
	Wire.begin();
}

void CommClass::loop()
{
    Wire.requestFrom(0x10, 16);
	State.actualSpeed[0] = WireRead;
	State.actualSpeed[1] = WireRead;
	while (Wire.available()) {
		char c = Wire.read();

	}
	State.actualRealSpeed[0] = WireRead;
	State.actualRealSpeed[1] = WireRead;
}


CommClass Comm;

