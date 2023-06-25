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
#include "globals.h"

extern volatile uint16_t valveADCValue;

void SetupValve(uint8_t storedValveLocation, uint8_t storedValveMode);

void SetNextValveLocation(uint8_t newValue);
uint8_t GetCurrentValveLocation(void);

void SetNextValveMode(uint8_t newMode);
uint8_t GetCurrentValveMode(void);

extern volatile uint8_t nextValveMode;
extern volatile uint8_t currentValveMode;

extern volatile uint16_t *ADC_Endstop_24_value;
extern volatile uint16_t *ADC_Endstop_0_value;

extern volatile uint8_t resetValve;

void CreateADCTable(void);

bool IsRemoteEnabled(void);
void MoveValveToNewPosition(void);

#endif	/* VALVE_MANAGER_H */

