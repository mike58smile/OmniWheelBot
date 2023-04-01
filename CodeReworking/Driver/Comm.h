/*****************************************************************//**
 * \file   Comm.h
 * \brief  Comm class header
 * \details Used for I2C communication with the Controller
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

#ifndef _COMM_h
#define _COMM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <i2cdetect.h>
#include <Wire.h>
class CommClass
{
 protected:


 public:
	void init();
	void requestEvent();
	void receiveData();
};

extern CommClass Comm;

#endif

