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

#ifndef SETTING_STATE_CONTROLLER_H
#define	SETTING_STATE_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "globals.h"
extern uint8_t valve_uid[VALVE_EEPROM_SERIAL_LEN];
extern uint8_t updateEEPROM;
/*
 Settings:
 * Valve Address
 * Ignore Relay Input
 * Service Mode remote lockout mode
 */
void SetAddress(uint8_t newAddress);
uint8_t GetAddress(void);


#endif	/* SETTING_STATE_CONTROLLER_H */

