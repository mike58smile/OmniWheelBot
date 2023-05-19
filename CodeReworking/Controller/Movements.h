// Movements.h

#ifndef _MOVEMENTS_h
#define _MOVEMENTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "State.h"
#include "Comm.h"

#define B 0.14 //%polomer robota v metroch
#define R 0.025 //%polomer kolesa v metroch
#define alfa1 HALF_PI 
#define alfa2 PI
#define alfa3 -HALF_PI
#define alfa4 0
class MovementsClass
{
 protected:
	 //float v = 0.3, w = 0;
	 //int alfa = 180;
	 float tempAlfa = 0;
	 float tempV = 0;
	 float spd1 = 0;
	 float spd2 = 0;
	 float spd3 = 0;
	 float spd4 = 0;
	 unsigned long ElapsedTime = 0;
	 unsigned long CurrentTime = 0;
	 bool circularMove = 0;
	 float alfaR;
	 float vy = 0.1, vx = -0.1;
	 float tg_alfaR = 0;
	 float w1, w2, w3, w4;
 public:
	void init();
	void circle(float spd, float radius);
	void calcSpd(float spd, int alfa, float w);
	void loop();
};

extern MovementsClass Movements;

#endif

