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

#ifndef VALVE_MANAGER_H
#define	VALVE_MANAGER_H

#include <xc.h> 
#include <stdbool.h>
#include "globals.h"


typedef enum
{
    VALVE_MODE_NORMAL       = 0x4,
    VALVE_MODE_SETTINGS     = 0x5,
    VALVE_MODE_MAINTAINENC  = 0x6
} ValveMode;

struct ValveSettings
{
    uint16_t endstop0ValueADC;
    uint16_t endstop24ValueADC;
};

struct ValveInfo
{
    ValveMode valveMode;
    bool enstop0Selected;
    uint8_t endstop0Value;
    uint8_t endstop24Value;
};

// Setup Valve
void SetupValve(ValveSettings *newValveSettings, ValveInfo *newValveInfo, uint8_t newMainPosition);

ValveMode SetNextValveMode(ValveMode newMode);
ValveMode GetCurrentValveMode(void);

uint8_t SetEndstop0Value(uint8_t newEndstopValue);
uint8_t GetEndstop0Value(void);
uint8_t SetEndstop24Value(uint8_t newEndstopValue);
uint8_t GetEndstop24Value(void);

uint8_t SetSelectedEndstop0(void);
uint8_t SetSelectedEndstop24(void);
uint8_t GetSelectedEndstopValue(void);

uint8_t PeriodicVerifyPosition(uint8_t overridePosition);

extern volatile uint16_t valveADCValue;

#endif	/* VALVE_MANAGER_H */

