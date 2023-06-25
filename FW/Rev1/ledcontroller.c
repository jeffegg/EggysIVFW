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

volatile LEDS display;
volatile LEDS next_display;

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

void SetLeds(void)
{
    
    if (nextValveMode == VALVE_MODE_NORMAL)
        next_display.LEDbits.AUTO_LED = 1;
    else
        next_display.LEDbits.AUTO_LED = 0;

    if (nextValveMode == VALVE_MODE_MAINTAINENCE)
        next_display.LEDbits.SERVICE_LED = 1;
    else
        next_display.LEDbits.SERVICE_LED = 0;

    if (nextValveMode == VALVE_MODE_SETTINGS)
        BlueModeLed_SetLow();
    else
        BlueModeLed_SetHigh();
    
    // If we are in remote mode (look only at remote bit since we keep track of the last setting)
    if ((nextValveMode & VALVE_MODE_REMOTE) == VALVE_MODE_REMOTE)
    {
        next_display.LEDbits.AUTO_LED = 1;
        next_display.LEDbits.SERVICE_LED = 0;
        BlueModeLed_SetLow();
    }
    
    uint8_t valveLocation = GetCurrentValveLocation();
    uint8_t ledToLight = valveLocation >> 2;
    uint8_t between2Lights = valveLocation & 0x1;
    
    switch(valveLocation)
    {
        case 0:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED0 = 1; 
            break;
        case 2:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED2 = 1; 
            break;
        case 4:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED4 = 1; 
            break;
        case 6:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED6 = 1;     
            break;
        case 8:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED8 = 1; 
            break;
        case 10:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED10 = 1; 
            break;
        case 12:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED12 = 1; 
            break;
        case 14:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED14 = 1;     
            break;
        case 16:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED16 = 1; 
            break;
        case 18:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED18 = 1; 
            break;
        case 20:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED20 = 1; 
            break;
        case 22:
            if (!between2Lights)
            {
                next_display.raw_leds = next_display.raw_leds & 0x7FFC;
            }
            next_display.LEDbits.LED22 = 1;   
            break;            
        case 24:
         if (!between2Lights)
         {
             next_display.raw_leds = next_display.raw_leds & 0x7FFC;
         }
         next_display.LEDbits.LED22 = 1;  
         break;
    }
}

void SetBlueModeLed(bool led_on)
{
  if (led_on != false)
      LATA &= 0x7f;
  else
      LATA |= 0x80;
  return;
}

