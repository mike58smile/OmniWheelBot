// SerialControl.h

#ifndef _SERIALCONTROL_h
#define _SERIALCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "State.h"

class SerialControlClass
{
 protected:
	 StateClass& State;
	 char EnableSerialMode = 'x';
	 String SerialString = "";
	 int SerialInt = 0;
 public:
	 SerialControlClass(StateClass& state) : State(state) {}
	 void setup();
	 void loop();
};

extern SerialControlClass SerialControl;

#endif

