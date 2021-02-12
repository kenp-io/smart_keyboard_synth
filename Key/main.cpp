/*
 * Copyright (c) 2017-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "can_api.h"
#include "mbed_retarget.h"
#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif


#include "mbed.h"
#include <inttypes.h>

using namespace std::chrono;

#define MAXSOFTPOTSTEPS     8191
#define MAXSOFTPOTPOS       10000
#define MAXPITCH            0x3fff
#define MINPITCH            0x0000


//CAN PARAMS
Ticker ticker;
DigitalOut led1(LED1);
CAN can(PA_11, PA_12, 500000);
char counter = 0;    
unsigned int  RX_ID = 0x101;
unsigned char canID;
//IR LED PARAMS
AnalogIn   readIrPin(A6);
DigitalOut irPin(D8);
unsigned short int irOnThresh [8] = {25000, 34000, 26500, 24500, 28000, 29500, 25000, 26000};
unsigned short int irOn;
//SOFTPOT PARAMS
AnalogIn   softPin(A3);
float step = 0;
//MIDI PARAMS
unsigned char baseOn;
unsigned char baseOff;
unsigned char notes [8] = {0x3c, 0x3e, 0x40, 0x41, 0x43, 0x45, 0x47, 0x48};
//unsigned char notes [8] = {0x18, 0x1a, 0x1c, 0x1d, 0x1f, 0x21, 0x23, 0x24};
//unsigned char notes [8] = {0x54, 0x56, 0x58, 0x59, 0x5b, 0x5d, 0x5f, 0x60};
unsigned char note;
unsigned char pitchChannel;
//SYS
uint32_t ID = HAL_GetUIDw0();
uint32_t keys [8] = {2147942409, 1245222, 1048610, 1703954, 2147680284, 1507336, 2147942422, 589869}; 

void initValues(){

    baseOn = 0x90;
    baseOff = 0x80;
    note = 0x3c;
    canID = 0x21;
    pitchChannel = 0xe0;
    irOn = 25000;

    for (int i = 0; i < 8; i++) {
        if (ID == keys[i]){
            RX_ID = RX_ID + i;
            baseOn = baseOn + i;
            baseOff = baseOff + i;
            note = notes[i];
            canID = canID + i;
            pitchChannel = pitchChannel + i;
            irOn = irOnThresh[i];
            break;
        }
    }
}

void printMsg(CANMessage msg){

    printf("Message sent: ");
    for(int i = 0; i < msg.len; i++)
    printf(" 0x%.2X", msg.data[i]);
    printf("\r\n");

}

void sendMsg(unsigned char msg1, unsigned char msg2, unsigned char msg3){

    CANMessage msg;
    msg.id = RX_ID;
    msg.type = CANData; 
    msg.format = CANStandard;
    msg.len = 3;

    msg.data[0] = msg1;
    msg.data[1] = msg2;
    msg.data[2] = msg3;

    for (int i = 0; i < 5; i++) {
        if (can.write(CANMessage(msg))) {
            printMsg(msg);
            break;
        }
        else {
            printf("reset\n");
            can.reset();
        }
        ThisThread::sleep_for(10ms);
    }

}

void sendPitch(unsigned short int pitch){
    unsigned  mask;
    mask = (1 << 7) - 1;
    sendMsg(pitchChannel,pitch & mask,(pitch>>7) & 0xff);
}

unsigned short int computePitchBend(unsigned short int oldSoft, unsigned short int newSoft, unsigned short int currentPitch){

    signed int bend;
    unsigned short int newPitch;

    //printf("oldSoft: %d\n", oldSoft);
    //printf("newSoft: %d\n", newSoft);
    bend = int((oldSoft-newSoft)*step);
    newPitch = currentPitch + bend;
    //printf("step: %d\n", step);
    //printf("bend: %d\n", bend);

    //printf("currentPitch: %x\n", currentPitch);
    //printf("newPitch: %x\n", newPitch);
    if (newPitch>0x3fff){
        newPitch = 0x3fff;
        if (currentPitch < 0x0100){
            newPitch = 0x0000;
        }
    }
    sendPitch(newPitch);

    return newPitch;
}

unsigned short int getPotPos(unsigned short int reading){

    unsigned short int constant;
    unsigned short int ref;
    unsigned short int dif;

    if (reading > 65000){
        constant = 5;
        ref = 10000;
        dif = reading - 65000;
    }
    else if (reading > 49000){
        constant = 28; 
        ref = 9000;
        dif = reading - 49000;       
    }
    else if (reading > 40000){
        constant = 18;
        ref = 8500;
        dif = reading - 40000;
    }
    else if (reading > 33000){
        constant = 14;
        ref = 8000;     
        dif = reading - 33000;   
    }
    else if (reading > 28000){
        constant = 10;
        ref = 7500;     
        dif = reading - 28000;   
    }
    else if (reading > 24500){
        constant = 7;
        ref = 7000;
        dif = reading - 24500;   
    }
    else{
        return reading/3.5;
    }

    return dif/constant + ref;
}

unsigned short int initPot(){

    unsigned short int iniPos = 0;

    iniPos = getPotPos(softPin.read_u16());
    printf("INIPOS %d \n", iniPos);
    if (iniPos<100){
        return 0;
    }

    if (iniPos>7500){
        step = float(iniPos) / MAXSOFTPOTSTEPS;
    }
    else if (iniPos>5000){
        step = float(MAXSOFTPOTPOS - iniPos) / MAXSOFTPOTSTEPS;
    }
    else if (iniPos>2500){
        step = float(iniPos) / MAXSOFTPOTSTEPS;
    }
    else{
        step = float(MAXSOFTPOTPOS - iniPos) / MAXSOFTPOTSTEPS;
    }
    printf("step: %d\n", int(step*1000));

    return iniPos;
}

unsigned char pressVelocity(unsigned short int timePress){

    float resolution;
    
    //times in ms
    if (timePress<5){
        return 127;
    }
    else if (timePress>200){
        return 20;
    }
    else{
        return -0.55*timePress+129.74;
    }
}

int main()
{

    printf("main()\n");

    //IR LED VARIABLES    
    irPin = 1;
    unsigned short int ir = 0;
    unsigned short int irRest = 0;
    unsigned char velocity;
    Timer t;
    unsigned int timePress  = 0;
    //MIDI
    bool noteOn = false;
    bool noteMoving = false;

    //SOFTPOT VARIABLES
    unsigned short int oldSoft = 0;
    unsigned short int newSoft = 0;
    unsigned short int iniPos = 0;
    unsigned short int basePitch = 0x2000;
    unsigned short int currentPitch = 0x2000;

    printf("ID:  %" PRIu32 "\n",ID);
    initValues();

    can.filter(canID, 0xFFF, CANStandard, 0);
    CANMessage msg;
    unsigned short int readTimer = 0;

    ThisThread::sleep_for(200ms);
    irRest = readIrPin.read_u16() + 1000;

    while (1) {

        ir = readIrPin.read_u16();
        //NOTE OFF BEHAVIOR
        if (!noteOn){
            if (ir > irRest && !noteMoving){
                t.start();
                noteMoving = true;
            }
            //KEY IS NOT PRESSED AND COMES BACK TO REST
            else if (noteMoving && ir < irRest){
                noteMoving = false;
                t.stop();
                t.reset();
            }
            //KEY PRESS DETECTION
            else if (ir > irOn){
                t.stop();
                timePress = int(duration_cast<milliseconds>(t.elapsed_time()).count());
                velocity = pressVelocity(timePress);
                t.reset();
                printf("The time on was %d milliseconds\n", timePress);
                sendMsg(baseOn, note, velocity);
                iniPos = initPot();
                oldSoft = iniPos;
                noteOn = true;
                noteMoving = false;
            }
        }
        //NOTE ON BEHAVIOR
        else {
            //FINGER IS REMOVED FROM KEY
            if (ir < irOn-1000 && !noteMoving){
                t.start();
                noteMoving = true;
            }
            else if (ir < irRest){
                t.stop();
                timePress = int(duration_cast<milliseconds>(t.elapsed_time()).count());
                velocity = pressVelocity(timePress);
                t.reset();
                printf("The time off was %d milliseconds\n", timePress);
                sendMsg(baseOff, note, velocity);
                sendPitch(basePitch);
                currentPitch = basePitch;
                noteOn = false;
                noteMoving = false;
            }
            //KEY IS BEING PRESSED
            else if (ir > irOn){
                //FALSE KEY REMOVAL
                if (noteMoving){
                    t.stop();
                    t.reset();
                }
                //we can't start pitch end before proper value read on softpot
                else if (!iniPos){
                    iniPos = initPot();
                    oldSoft = iniPos;
                }
                else {
                    newSoft = getPotPos(softPin.read_u16());
                    //read error, finger not placed properly
                    if (newSoft==0){
                        newSoft = oldSoft;
                    }
                    else if (newSoft != oldSoft and abs(int(newSoft)-int(oldSoft))<3000){
                        currentPitch = computePitchBend(oldSoft,newSoft,currentPitch);
                        oldSoft = newSoft;
                    }
                    //DON'T WANT TO SEND TOO MUCH PITCH BEND INFO
                    ThisThread::sleep_for(20ms);
                }
            }
        }

        //We only read from time to time to improve performances
        //Indeed, they only received messages are to flash a new firmware, latency is not a priority
        readTimer = readTimer + 1;
        if (readTimer == 100){
            if (can.read(msg)) {
                //printf("Message received: %d\n", msg.data[0]);
                printf("  ID      = 0x%.3x\r\n", msg.id);
                if (msg.id == canID){
                    printf("slayy\n");
                    system_reset();
                }
            }
            readTimer = 0;
        }

        //led1 = !led1;   
        ThisThread::sleep_for(1ms);
        }
}