/*****************************************************************//**
 * \file   Driver.ino
 * \brief  Main program file
 * \details Here is void setup() and void loop()
 * 
 * \author xmisko06
 * \date   April 2023
 *********************************************************************/

//#include <Wire.h>
#include "State.h" // Include before Drive.h!
#include "Wire.h"
#include "Comm.h"
//#include "Motors.h"
#include "Drive.h"
//#include <Encoder.h>

#include <ISR_Timer.h>
#include <TimerInterrupt.hpp>
#include <TimerInterrupt.h>
#include <ISR_Timer.hpp> 

constexpr auto BaudRate = 115200;
#define WireRead (Wire.read() | Wire.read() << 8)

#define WireWrite(int16){ \
Wire.write(int16); \
Wire.write((int16 >> 8)); \
}

StateClass State;
CommClass Comm(State);
DriveClass Drive(State);
//CommClass Comm;
//DriveClass Drive;

//volatile int actualSpeed[2] = { 0,0 }
unsigned int encSpeed[2] = { 0,0 };
//volatile float actualRealSpeed[2] = {
//float requiredRealSpeed[2] = { 0,0 };
volatile bool flag = 0;
volatile int ii;
void TimerSpeedHandler()
{
   // encSpeed[0] = abs(Drive.enc1.read());
    //Drive.enc1.write(0);
    //encSpeed[1] = abs(Drive.enc2.read());
    //Drive.enc2.write(0);
   // if (!flag) {
        flag = 1;
        ++ii;
    //}
    //else{
    //    flag = 0;
    //}
    //State.actualRealSpeed[0] = (State.encSpeed[0] * 2 * PI) / (979.2 * (TimerSpeedDelayMS / 1000));
    //State.actualRealSpeed[1] = (State.encSpeed[1] * 2 * PI) / (979.2 * (TimerSpeedDelayMS / 1000));
}
/**
 * \brief the setup function runs once when you press reset or power the board
 */

void requestEvent()
{
    WireWrite(State.actualSpeed[0]);
    WireWrite(State.actualSpeed[1]);
    char strBuffer[7];
    Wire.write(dtostrf(State.actualRealSpeed[0], 7, 2, strBuffer)); //!odskusat ci funguje
    Wire.write(dtostrf(State.actualRealSpeed[1], 7, 2, strBuffer));//!odskusat ci funguje  
}

void receiveData(int x)
{
    int mode = Wire.read();
    switch (mode) {
    case 0:
        //stop motor (and reset PID)
        State.commState = CommState::Stop;
        break;
    case 1:
        State.commState = CommState::Wait;
        break;
    case 2:
        //set speed
        State.requiredSpeed[0] = WireRead;
        State.requiredSpeed[1] = WireRead;
        State.commState = CommState::SpeedPWM;
        break;
    case 3:
        State.requiredRealSpeed[0] = WireRead;
        State.requiredRealSpeed[1] = WireRead;
        State.commState = CommState::SpeedReal;
    default:
        State.commState = CommState::Unknown;
        break;
    }
}


void setup() {
    // Timer init
    ITimer2.init();
    //ITimer1.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler);
    if (ITimer2.attachInterruptInterval(TimerSpeedDelayMS, TimerSpeedHandler))
    {
        Serial.println(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
    }
    else
        Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
    Serial.begin(BaudRate);
    Wire.begin(State.address);
	//Comm.init();
   
    Wire.onReceive(receiveData);
    Wire.onRequest(requestEvent);
	Drive.init();
}

/**
 * \brief The loop function which runs over and over again until power down or reset
 */
    unsigned long currentMillis_1 = 0;
    unsigned long previousMillis_1 = 0;
void loop() {
//    currentMillis_1 = millis();
//    if ((currentMillis_1 - previousMillis_1) >= 200) {
//        encSpeed[0] = abs(Drive.enc1.read());
//        Drive.enc1.write(0);
//        encSpeed[1] = abs(Drive.enc2.read());
//        Drive.enc2.write(0);
//;       previousMillis_1 = currentMillis_1;
//    }
    
	//Drive.loop();

    if (flag) {
        encSpeed[0] = abs(Drive.enc1.read());
        Drive.enc1.write(0);
        encSpeed[1] = abs(Drive.enc2.read());
        Drive.enc2.write(0);
        flag = 0;
        ii = 0;
    }
    analogWrite(State.M1_LPWM, 0);
    analogWrite(State.M1_RPWM, abs(50));
    analogWrite(State.M2_LPWM, 0);
    analogWrite(State.M2_RPWM, abs(50));

}
