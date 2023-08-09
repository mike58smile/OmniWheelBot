/*****************************************************************//**
 * \file   Movements.h
 * \brief  Movements class header
 * \details Class implementing kinematics calculations and advanced movements
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#ifndef _MOVEMENTS_h
#define _MOVEMENTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "State.h"
#include "Comm.h" // Also includes Wire.h
#include <PID_v1.h>
//#include "Gyro.h"
#include "NewGyro.h"

#define B 0.14 //chassis radius [m]
#define R 0.025 // wheel radius [m]
#define alfa1 HALF_PI 
#define alfa2 PI
#define alfa3 -HALF_PI
#define alfa4 0
class MovementsClass
{
 protected:

	 //********For speed calculations - calcSpd*************
	 float tempAlfa = 0;
	 unsigned long ElapsedTime = 0;
	 unsigned long CurrentTime = 0;
	 float alfaR;
	 float vy, vx;
	 float tg_alfaR;
	 float w1, w2, w3, w4;

	 //********For gyro PID*************************
	 
	 double pid_Set = 0, pid_In = 0, pid_Out = 0; ///< Define signals for PID gyro reg
	 NewGyroClass gyro;
	 unsigned long timer = 0;
 public:
	 PID pid;

	MovementsClass(): pid(&pid_In, &pid_Out, &pid_Set, State.Kp, State.Ki, State.Kd, DIRECT){}

	void init();
	void circle(float spd, float radius, float w);
	void calcSpd(float spd, int alfa, float w);
	void gyroTest();
	void gyroPid(); ///< call in loop to work
	void loop();
};

extern MovementsClass Movements;

#endif

