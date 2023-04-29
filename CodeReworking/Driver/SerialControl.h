// SerialControl.h

#ifndef _SERIALCONTROL_h
#define _SERIALCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SerialControlClass
{
 protected:

 public:
	void init();
};

extern SerialControlClass SerialControl;

#endif

