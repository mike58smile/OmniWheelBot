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

void setRealSingle(int motSelect, float spd) {
    switch (motSelect) {
    case 0:
        Comm.SetReal(spd, 0, 0, 0);
        break;
    case 1:
        Comm.SetReal(0, spd, 0, 0);
        break;
    case 2:
        Comm.SetReal(0, 0, spd, 0);
        break;
    case 3:
        Comm.SetReal(0, 0, 0, spd);
        break;
    }
}

void IRClass::control()
{
    using namespace IR_denon; //Using DENON IR remote control
    bool readIR = read();
    /** How to add mode?
    * Add mode "YourMode" to State_movement enum
    * Add case to Main mode automat (the one below)
    * Use this construct:
    * if (State.state_movement == State_movement::YourMode) {
    *    //use this to define operations which should be done only once when the button in firt pressed
    *    if (readIR) {
    *       switch (currentRecievedFlag) {
            case NUM_1:
                //what to do when once pressed NUM_1
                break;
    *       }
    *    }
    *    
    *    //use this to define operations which should be done repeatingly - like increment variable
    *    //ignores 100 ms between last click - in that time it is FALSE, otherwise TRUE
    *    if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
    *        switch (currentRecievedFlag) {
             case NUM_1:
                //what to do when once pressed NUM_1
                break;
    *       }
    *    }
    * }
    */


    // Main mode choose automat
    if (readIR) {
        switch (currentRecievedFlag) { //could be replaced with IrReceiver.decodedIRData.decodedRawData
        case STANDBY_MAIN:
            State.GyroRegON = 0;
            break;
        case ON_MAIN:
            State.GyroRegON = 1;
            break;
        case MUTE:
            //State.state_movement = State_movement::Meas;
            State.state_movement = State_movement::ShowTime_Dir;
            break;
        case PLAY:
            State.state_movement = State_movement::ShowTime_Circ;
            break;
        case INTERNET_RADIO:
            State.state_movement = State_movement::IR_movement;
            break;
        case PRESET_1:
            State.state_movement = State_movement::CalcSpd;
            break;
        case PRESET_2:
            State.state_movement = State_movement::Circle;
            break;
        case PRESET_3:
            State.state_movement = State_movement::PidGyro;
            break;
        case STOP:
            State.state_movement = State_movement::Meas;
            break;
        case PAUSE:
            State.state_movement = State_movement::MeasTypes;
            break;
        }
    }

    if (State.state_movement == State_movement::MeasTypes) {
        if (readIR) {
            switch (currentRecievedFlag) {
            case NUM_1:
                Comm.SetMeas(MeasType::Ramp);
                break;
            case NUM_2:
                Comm.SetMeas(MeasType::Ramp_optim);
                break;
            case NUM_3:
                Comm.SetMeas(MeasType::Calib);
                break;
            }
        }
    }

    if (State.state_movement == State_movement::ShowTime_Dir) {
        if (readIR) {
            switch (currentRecievedFlag) {
            case UP: //UP
                Serial.println("UP");
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 0;
                break;
            case CH_LEVEL:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 45;
                break;
            case RIGHT:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 90;//225;
                break;
            case RETURN:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 135;
                break;
            case DOWN:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 180;
                break;
            case SEARCH:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 225;
                break;
            case LEFT:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 270;
                break;
            case MENU:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 315;
                break;
            case FORWARD:
                if (!State.wantedW || State.wantedW > 0) State.wantedW = -1.3;
                break;
            case BACKWARD:
                if (!State.wantedW || State.wantedW < 0) State.wantedW = 1.3;
                break;
            case ENTER: //ENTER
                Serial.println("ENTER");
                State.wantedV = 0;
                State.wantedW = 0;
                break;
            }
        }
        if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
            switch (currentRecievedFlag) {
            case VOL_UP:
                State.wantedV += 0.05;
                break;
            case VOL_DOWN:
                State.wantedV -= 0.05;
                break;
            case CH_UP:
                State.wantedW += 0.1;
                break;
            case CH_DOWN:
                State.wantedW -= 0.1;
                break;
            case FAST_FORWARD:
                State.gyroUpdateMS += 10;
                break;
            case FAST_BACKWARD:
                State.gyroUpdateMS -= 10;
                break;
            }
        }
    }

    if (State.state_movement == State_movement::ShowTime_Circ) {
        if (readIR) {
            switch (currentRecievedFlag) {
            case ENTER:
                State.wantedV = 0;
                State.wantedW = 0;
                State.stopCirc = 1;
                Serial.println(" -------------------------------------------- ");
                break;
            case NUM_1:
                State.wantedV = 0.45;
                State.wantedRadius = 0.4;
                break;
            case NUM_2:
                State.wantedV = 0.225;
                State.wantedRadius = 0.4;
                break;
            case NUM_3:
                State.wantedV = 0.225;
                State.wantedRadius = 0.2;
                break;
            case NUM_4:
                State.wantedV = 0.225;
                State.wantedRadius = 0.2;
                break;
            case NUM_5:
                State.wantedV = 0.225;
                State.wantedRadius = 0.1;
            }
        }
        if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
            switch (currentRecievedFlag) {
            case VOL_UP:
                State.wantedV += 0.05;
                break;
            case VOL_DOWN:
                State.wantedV -= 0.05;
                break;
            case CH_UP:
                State.wantedRadius += 0.01;
                break;
            case CH_DOWN:
                State.wantedRadius -= 0.01;
                break;
            }
        }
        if (State.stopCirc && (State.wantedW != 0 || State.wantedV != 0))
            State.stopCirc = 0;
    }


    if (State.state_movement == State_movement::Meas) {
        if (readIR) {
            switch (currentRecievedFlag) {
            case ENTER:
                Comm.SetReal(0, 0, 0, 0);
                break;
            case NUM_4:
                setRealSingle(State.motSelectMeas, 8);
                //Comm.SetReal(8, 0, 0, 0);
                break;
            case NUM_5:
                setRealSingle(State.motSelectMeas, 10);
                //Comm.SetReal(10, 0, 0, 0);
                break;
            case NUM_6:
                setRealSingle(State.motSelectMeas, 12);
                //Comm.SetReal(12, 0, 0, 0);
                break;
            case CH_UP:
                State.motSelectMeas = 0;
                break;
            case CH_DOWN:
                State.motSelectMeas = 1;
                break;
            case VOL_UP:
                State.motSelectMeas = 2;
                break;
            case VOL_DOWN:
                State.motSelectMeas = 3;
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
            }
        }
    }


    if (State.state_movement == State_movement::IR_movement) {
        if (readIR) { //True only once when new button pressed
            switch (currentRecievedFlag) { //could be replaced with IrReceiver.decodedIRData.decodedRawData
            case UP: //UP
                Serial.println("UP");
                //Comm.SetRealEnc(realSpdBegin, realSpdBegin, 0, 0);
                Comm.SetReal(8, 0, 0, 0);
                break;
            case ENTER: //ENTER
                Serial.println("ENTER");
                Comm.SetRealEnc(0, 0, 0, 0);
                //Comm.Stop();
                break;
            case DOWN: //DOWN
                Serial.println("DOWN");
                Comm.SetReal(0.2,0.2,0.2,0.2);
                //Comm.SetRealEnc(35, 0, 0, 0);
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
            if (currentRecievedFlag == 0xF001B140)
                Comm.SetPWM(30);
            if (currentRecievedFlag == 0x8001C140)
                Comm.SetPWM(50);
        }
    }
    else if (State.state_movement == State_movement::MeasGyro || State.state_movement == State_movement::PidGyro) {
        if (readIR) {
            switch (currentRecievedFlag) {
            case LEFT:
                State.wantedW = (State.wantedW == 0) ? 0.8 : signF(State.wantedW) * 0.8; //75000 NA VYSTUPE
                break;
            case UP:
                State.wantedW = (State.wantedW == 0) ? 1 : signF(State.wantedW) * 1;
                break;
            case RIGHT:
                State.wantedW = (State.wantedW == 0) ? 1.5 : signF(State.wantedW) * 1.5;
                break;
            case ENTER:
                State.wantedW = 0;
                break;
            case VOL_UP:
                State.wantedW = 1.0 * float(abs(State.wantedW));
                break;
            case VOL_DOWN:
                State.wantedW = -1.0 * float(abs(State.wantedW));
                break;
            }
        }

        if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
            switch (currentRecievedFlag) {
            case NUM_1:
                State.Kp += 0.1;
                break;
            case NUM_2:
                State.Ki += 0.1;
                break;
            case NUM_3:
                State.Kd += 0.1;
                break;
            case NUM_7:
                State.Kp -= 0.1;
                break;
            case NUM_8:
                State.Ki -= 0.1;
                break;
            case NUM_9:
                State.Kd -= 0.1;
                break;
            case CH_UP:
                State.gyroUpdateMS += 10;
                break;
            case CH_DOWN:
                State.gyroUpdateMS -= 10;
                break;
            }
        }
    }
    else if (State.state_movement == State_movement::Circle || State.state_movement == State_movement::CalcSpd) {
        if (readIR) { //True only once when new button pressed
            switch (currentRecievedFlag) { //could be replaced with IrReceiver.decodedIRData.decodedRawData
            case UP: //UP
                Serial.println("UP");
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 0;
                break;
            case CH_LEVEL:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 45;
                break;
            case RIGHT:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 90;//225;
                break;
            case RETURN:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 135;
                break;
            case DOWN:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 180;
                break;
            case SEARCH:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 225;
                break;
            case LEFT:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 270;
                break;
            case MENU:
                if (!State.wantedV) State.wantedV = 0.2;
                State.wantedAlfa = 315;
                break;
            case ENTER: //ENTER
                Serial.println("ENTER");
                State.wantedV = 0;
                //Comm.Stop();
                break;
            case NUM_1:
                State.wantedV = 0.45;
                State.wantedRadius = 0.4;
                break;
            case NUM_2:
                State.wantedV = 0.6;
                State.wantedRadius = 0.4;
                break;
            case NUM_3:
                State.wantedV = 0.5;
                State.wantedRadius = 0.2;
                break;
            case NUM_4:
                State.wantedV = 0.3;
                break;
            case NUM_5:
                State.wantedV = 0.45;
                break;
            case NUM_6:
                State.wantedW = 1.5;
                break;
            case NUM_7:
                State.wantedV = 0.5;
                State.wantedRadius = 0.2;
                break;
            case NUM_8:
                State.wantedV = 0.5;
                State.wantedRadius = 0.1;
                break;
            case NUM_9:
                State.wantedV = 0.5;
                State.wantedRadius = 0.4;
                break;
            }
        }

        if (isSingleClick) { //true after pressed for 100ms (and kinda also on beginning)
            switch (currentRecievedFlag) {
            case VOL_UP:
                State.wantedV += 0.05;
                break;
            case VOL_DOWN:
                State.wantedV -= 0.05;
                break;
            case CH_UP:
                State.wantedRadius += 0.01;
                break;
            case CH_DOWN:
                State.wantedRadius -= 0.01;
                break;
            }
        }

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