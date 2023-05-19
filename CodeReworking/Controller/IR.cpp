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

bool IRClass::detectNextPress(uint16_t aLongPressDurationMillis)
{
    if (!sLongJustPressed2 && (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        /*
            * Here the repeat flag is set (which implies, that command is the same as the previous one)
            */
        if (millis() - aLongPressDurationMillis > sMillisOfFirstReceive2) {
            sLongJustPressed2 = true; // Long press here
        }
    }
    else {
        // No repeat here
        sMillisOfFirstReceive2 = millis();
        sLongJustPressed2 = false;
    }
    return sLongJustPressed2; // No long press here
}
void IRClass::control()
{
    using namespace IR_denon; //Using DENON IR remote control
    bool readIR = read();
    if (readIR) {
        switch (currentRecievedFlag) { //could be replaced with IrReceiver.decodedIRData.decodedRawData
        case UP: //UP
            Serial.println("UP");
            Comm.SetRealEnc(realSpdBegin, realSpdBegin,0,0);
            break;
        case ENTER: //ENTER
            Serial.println("ENTER");
            Comm.SetRealEnc(0, 0, 0, 0);
            //Comm.Stop();
            break;
        case DOWN: //DOWN
            Serial.println("DOWN");
            Comm.SetRealEnc(35,0,0,0);
            break;
        }
    }

    if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
        switch (currentRecievedFlag) {
        case NUM_1:
            Comm.SetPID(add, 0, 0);
            break;
        case NUM_2:
            Comm.SetPID(0, add, 0);
            break;
        case NUM_3:
            Comm.SetPID(0, 0, add);
            break;
        case NUM_7:
            Comm.SetPID(-add, 0, 0);
            break;
        case NUM_8:
            Comm.SetPID(0, -add, 0);
            break;
        case NUM_9:
            Comm.SetPID(0, 0, -add);
            break;
        case VOL_UP:
            ++realSpdBegin;
            Comm.SetRealEnc(realSpdBegin, realSpdBegin, 0, 0);
            break;
        case VOL_DOWN:
            --realSpdBegin;
            Comm.SetRealEnc(realSpdBegin, realSpdBegin, 0, 0);
            break;
        }
    }
    //Serial.println(currentRecievedFlag);
    if (LongPressFlag) {
        Serial.print("LONG PRESSED: ");
        Serial.println(currentRecievedFlag, HEX);
        if(currentRecievedFlag == 0xF001B140)
            Comm.SetPWM(30);
        if(currentRecievedFlag == 0x8001C140)
            Comm.SetPWM(50);
    }
}
bool tempReceived = 0;
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

        if (detectNextPress(70))
            isSingleClick = 1;
        if (detectLongPress(1000)) {
            Serial.print(F("Command 0x"));
            Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
            Serial.println(F(" was repeated for more than 2 seconds"));
            LongPressFlag = 1;
        }
        return !isSameRecieved;
    }
    else {
        isSingleClick = 0;
        LongPressFlag = 0;
        return 0;
    }
}

IRClass IR(State);