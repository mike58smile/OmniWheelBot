/*****************************************************************//**
 * \file   IR.cpp
 * \brief  IR class source file
 * \details Class handling recieving IR signal
 * 
 * \author xmisko06
 * \date   May 2023
 *********************************************************************/

#include "IR.h"
#include <IRremote.hpp>

void IRClass::init()
{
    IrReceiver.begin(State.IRPin, ENABLE_LED_FEEDBACK); // Start the receiver
}
uint32_t IRClass::test() {
    if (IrReceiver.decode()) {
        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
        /* USE NEW 3.x FUNCTIONS */
        IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
        IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
        IrReceiver.resume(); // Enable receiving of the next value
    }
}
IRClass IR(State);