/*
    EggysIVFW - Custom Firmware for Pentair's Intellivalve (TM)
    Copyright (C) 2021-2023  Jeff "Eggy" Eglinger

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
   
 */

#include <xc.h>
#include <stdbool.h>
#include "ledcontroller.h"
#include "globals.h"
#include "mcc_generated_files/mcc.h"
#include "command_system.h"
#include "eeprom_controller.h"
#include "valve_manager.h"
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

volatile LEDS currentDisplay;
volatile LEDS nextDisplay;

bool nextIDValve = false;
bool currentIDValve = false;

void SetupLeds(void)
{    
    nextDisplay.raw_leds = 0;
    currentDisplay.raw_leds = nextDisplay.raw_leds;
}

void UpdateLeds(void)
{
    
    if (currentDisplay.raw_leds != nextDisplay.raw_leds)
    {
        uint8_t delayLoop1 = 0;
        uint8_t delayLoop2 = 0;
        uint16_t ledValue = nextDisplay.raw_leds;

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

        currentDisplay.raw_leds =  nextDisplay.raw_leds;
    }
    return;
}

void SetLeds(void)
{    
    ValveMode nextValveMode = GetCurrentValveMode();
    if (nextValveMode == VALVE_MODE_NORMAL)
        nextDisplay.LEDbits.AUTO_LED = 1;
    else
        nextDisplay.LEDbits.AUTO_LED = 0;

    if (nextValveMode == VALVE_MODE_MAINTAINENC)
        nextDisplay.LEDbits.SERVICE_LED = 1;
    else
        nextDisplay.LEDbits.SERVICE_LED = 0;

    if (nextValveMode == VALVE_MODE_SETTINGS)
        BlueModeLed_SetLow();
    else
        BlueModeLed_SetHigh();
    
    
    uint8_t valveLocation = GetCurrentPosition();
    uint8_t ledToLight = valveLocation >> 2;
    uint8_t between2Lights = valveLocation & 0x1;
    nextDisplay.raw_leds = 0;
    switch(ledToLight)
    {
        case 0:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED0 = 1; 
            break;
        case 1:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED2 = 1; 
            break;
        case 2:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED4 = 1; 
            break;
        case 3:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED6 = 1;     
            break;
        case 4:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED8 = 1; 
            break;
        case 5:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED10 = 1; 
            break;
        case 6:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED12 = 1; 
            break;
        case 7:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED14 = 1;     
            break;
        case 8:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED16 = 1; 
            break;
        case 9:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED18 = 1; 
            break;
        case 0xa:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED20 = 1; 
            break;
        case 0xb:
            if (between2Lights)
            {
                nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
            }
            nextDisplay.LEDbits.LED22 = 1;   
            break;            
        case 0xc:
         if (between2Lights)
         {
             nextDisplay.raw_leds = currentDisplay.raw_leds & 0x7FFC;
         }
         nextDisplay.LEDbits.LED24 = 1;  
         break;
        case 0xFF:
        default:
            nextDisplay.raw_leds = 0x7FFC; //Turn on all LEDS to signify error
         break;
    }
}

void IdentifyValve(void)
{
    nextIDValve = true;
}

