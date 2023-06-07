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
#ifndef EEPROM_CONTROLLER_H
#define	EEPROM_CONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  

enum {VALVE_EEPROM_SIZE = 0x80, VALVE_EEPROM_SERIAL_ADDR = 0xFA};
const uint8_t VALVE_EEPROM_ADDRESS = 0xA0;
const uint8_t VALVE_EEPROM_ADDRESS_SHIFTED = VALVE_EEPROM_ADDRESS >> 1;

void DumpEEPROMtoMemory(uint8_t *eeprom_data);
void GetUUID(uint8_t *valve_uid);

void WriteEEPROM(uint8_t addr, uint8_t value);
void WriteEEPROMBuffer(uint8_t eeprom_addr, uint8_t *buffer, uint8_t length);
uint16_t CheckSumMaker(uint8_t *buffer, uint8_t size);

#endif	/* EEPROM_CONTROLLER_H */