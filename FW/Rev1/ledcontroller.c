#include "ledcontroller.h"
// 0 -> AUTO
// 1 -> OFF
// 2 -> 10
// 3 -> 8
// 4 -> 6
// 5 -> 4
// 6 -> 2
// 7 -> 0
// 8 -> 24
// 9 -> 22
// 10 -> 20
// 11 -> 18
// 12 -> 16
// 13 -> 14
// 14 -> 12
// 15 -> Service

void ControlLights(LEDS *param_0)
{
    uint8_t delayLoop1 = 0;
    uint8_t delayLoop2 = 0;
    uint16_t ledValue = param_0->raw_leds;
    
    SSP1CON1 = 8;
    RC3PPS = RC3PPS & 0xe0;
    RC4PPS = RC4PPS & 0xe0;
    SSPCLKPPS = 0;
    SSPDATPPS = 0;
    TRISC &= 0xef;
    TRISC &= 0xf7;
    LATC &= 0xf7;

    delayLoop1 = 6;
    delayLoop2 = 0x30;
    do {
        do {
            delayLoop2 -= 1;
        } while (delayLoop2 != 0);
        delayLoop1 -= 1;
    } while (delayLoop1 != 0);

    for (uint8_t i = 0; i < 0x10; i += 1) {
        if ((( ledValue >> i) & 0x1) == 0) {
            LATC &= 0xef;
        }
        else {
            LATC |= 0x10;
        }

        delayLoop1 = 0x1a;
        do {
            delayLoop1 -= 1;
        } while (delayLoop1 != 0);

        LATC |= 8;

        delayLoop1 = 6;
        delayLoop2 = 0x30;
        do {
            do {
                delayLoop2 -= 1;
            } while (delayLoop2 != 0);
            delayLoop1 -= 1;
        } while (delayLoop1 != 0);

        LATC &= 0xf7;
    }

    LATC &= 0xef;
    LATC |= 0x20;

    delayLoop1 = 0x1a;
    do {
        delayLoop1 -= 1;
    } while (delayLoop1 != 0);

    LATC &= 0xdf;
    RC3PPS = (RC3PPS & 0xe0) | 0x10;
    RC4PPS = (RC4PPS & 0xe0) | 0x11;
    SSPCLKPPS = 0x13;
    SSPDATPPS = 0x14;

    TRISC |= 0x10;
    TRISC |= 8;
    SSP1CON1 = 0x28;

    return;
}
