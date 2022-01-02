#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <cstring>
#include <cctype>

#ifndef FRAMEHANDLER_H_
#define FRAMEHANDLER_H_

const uint8_t frameLen = 22;                       // VE.Direct Protocol: max frame size is 18
const uint8_t nameLen = 9;                         // VE.Direct Protocol: max name size is 9 including /0
const uint8_t valueLen = 33;                       // VE.Direct Protocol: max value size is 33 including /0
const uint8_t buffLen = 40;                        // Maximum number of lines possible from the device. Current protocol shows this to be the BMV700 at 33 lines.


class VeDirectFrameHandler {

public:
    VeDirectFrameHandler();
    void rxData(uint8_t inbyte);                // byte of serial data to be passed by the application

    char veName[buffLen][nameLen] = { };        // public buffer for received names
    char veValue[buffLen][valueLen] = { };      // public buffer for received values

    int frameIndex;                             // which line of the frame are we on
    int veEnd;                                  // current size (end) of the public buffer

private:
    //bool mStop;                               // not sure what Victron uses this for, not using

    enum States {                               // state machine
        IDLE,
        RECORD_BEGIN,
        RECORD_NAME,
        RECORD_VALUE,
        CHECKSUM,
        RECORD_HEX
    };

    int mState;                                 // current state

    uint8_t	mChecksum;                          // checksum value

    char * mTextPointer;                        // pointer to the private buffer we're writing to, name or value

    char mName[9];                              // buffer for the field name
    char mValue[33];                            // buffer for the field value
    char tempName[frameLen][nameLen];           // private buffer for received names
    char tempValue[frameLen][valueLen];         // private buffer for received values

    void textRxEvent(char *, char *);
    void frameEndEvent(bool);
    void logE(char *, char *);
    bool hexRxEvent(uint8_t);
};

#endif // FRAMEHANDLER_H_