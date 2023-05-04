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
    Wire.requestFrom(State.adress[0], 16);
	State.actualSpeed[0] = WireRead();
	State.actualSpeed[1] = WireRead();
	State.actualRealSpeed[0] = WireRead();
	State.actualRealSpeed[1] = WireRead();
}

CommClass Comm(State);

