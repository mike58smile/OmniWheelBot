/*****************************************************************//**
 * \file   IR.cpp
 * \brief  IR class source file
 * \details Class handling recieving IR signal
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#include "IR.h"
//#define DECODE_DENON
#include <IRremote.hpp>
#include "Comm.h"
void IRClass::init()
{
    IrReceiver.begin(State.IRPin, ENABLE_LED_FEEDBACK); // Start the receiver
}
uint32_t IRClass::test() {
    if (IrReceiver.decode()) {
        IrReceiver.resume(); // Enable receiving of the next value
        currentRecievedFlag = IrReceiver.decodedIRData.decodedRawData;
        if (lastRecieved != currentRecievedFlag) {
            lastRecieved = currentRecievedFlag;
            Serial.println(currentRecievedFlag, HEX); // Print "old" raw data
            /* USE NEW 3.x FUNCTIONS */
            IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
            Serial.println(IrReceiver.decodedIRData.extra);
            //IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
        }
        if (detectLongPress(1000)) {
            Serial.print(F("Command 0x"));
            Serial.print(IrReceiver.decodedIRData.command, HEX);
            Serial.println(F(" was repeated for more than 2 seconds"));
        }
    }
}

bool IRClass::detectLongPress(uint16_t aLongPressDurationMillis)
{
    if (!sLongJustPressed && (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        /*
            * Here the repeat flag is set (which implies, that command is the same as the previous one)
            */
        if (millis() - aLongPressDurationMillis > sMillisOfFirstReceive) {
            sLongJustPressed = true; // Long press here
        }
    }
    else {
        // No repeat here
        sMillisOfFirstReceive = millis();
        sLongJustPressed = false;
    }
    return sLongJustPressed; // No long press here
}

void IRClass::control()
{
    using namespace IR_denon; //Using DENON IR remote control
    bool readIR = read();
    if (readIR) {
        switch (currentRecievedFlag) { //could be replaced with IrReceiver.decodedIRData.decodedRawData
        case UP: //UP
            Serial.println("UP");
            Comm.SetPWM(20);
            break;
        case ENTER: //ENTER
            Serial.println("ENTER");
            Comm.Stop();
            break;
        case DOWN: //DOWN
            Serial.println("DOWN");
            Comm.SetPID(0.5, 0.5, 0.5);
            break;
        }
    }
    if (LongPressFlag) {
        Serial.print("LONG PRESSED: ");
        Serial.println(currentRecievedFlag, HEX);
        if(currentRecievedFlag == 0xF001B140)
            Comm.SetPWM(30);
        if(currentRecievedFlag == 0x8001C140)
            Comm.SetPWM(50);
    }
}

bool IRClass::read()
{
    LongPressFlag = 0;
    if (IrReceiver.decode()) {
        IrReceiver.resume(); // Enable receiving of the next value
        currentRecievedFlag = IrReceiver.decodedIRData.decodedRawData;
        if (lastRecieved != currentRecievedFlag) {
            lastRecieved = currentRecievedFlag;
            isSameRecieved = 0;
        }
        else {
            isSameRecieved = 1;
        }
        if (detectLongPress(1000)) {
            //Serial.print(F("Command 0x"));
            //Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
            //Serial.println(F(" was repeated for more than 2 seconds"));
            LongPressFlag = 1;
        }
        return !isSameRecieved;
    }
    else {
        LongPressFlag = 0;
        return 0;
    }
}

IRClass IR(State);