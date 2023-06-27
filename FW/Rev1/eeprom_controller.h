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

// Custom FW EEPROM
const uint8_t VALVE_EEPROM_RS485_ADDRESS                        = 0x60;  // RS485 valve address location 1 byte
const uint8_t VALVE_EEPROM_MODE_ADDRESS                         = 0x61;  // Current Valve Mode 1 byte
const uint8_t VALVE_EEPROM_0_END_STOP_ADDRESS                   = 0x62;  // Current Valve Mode 1 bytes + Reserve 1 byte
const uint8_t VALVE_EEPROM_24_END_STOP_ADDRESS                  = 0x64;  // Current Valve Mode 1 bytes + Reserve 1 byte
const uint8_t VALVE_EEPROM_SELECTED_END_STOP_ADDRESS            = 0x66;  // Current endstop selected 1 byte
const uint8_t VALVE_EEPROM_MAINTENCE_POSITION                   = 0x67;  // Current endstop selected 1 byte
const uint8_t VALVE_EEPROM_DEBUG_LEVEL_ADDRESS                  = 0x68;  // Current debug level 4 bytes

// Production FW EEPROM Location - 
// No plans to use outside of:  VALVE_EEPROM_PROD_FIRMWARE_UPDATE_ADDRESS (Enter FW update)
//                              VALVE_EEPROM_PROD_DEVICE_DID_ADDRESS (Device ID )
//                              VALVE_EEPROM_PROD_DEVICE_RID_ADDRESS (Revision ID)
const uint8_t VALVE_EEPROM_PROD_0_ENDSTOP_ADDRESS               = 0x1;
const uint8_t VALVE_EEPROM_PROD_24_ENDSTOP_ADDRESS              = 0x3;
const uint8_t VALVE_EEPROM_PROD_BACKUP_0_ENDSTOP_ADDRESS        = 0x20;
const uint8_t VALVE_EEPROM_PROD_BACKUP_24_ENDSTOP_ADDRESS       = 0x22;
const uint8_t VALVE_EEPROM_PROD_SELECTED_ENDSTOP_ADDRESS        = 0x4;
const uint8_t VALVE_EEPROM_PROD_BACKUP_SELECTED_ENDSTOP_ADDRESS = 0x23;
const uint8_t VALVE_EEPROM_PROD_MODE_ADDRESS                    = 0x5; // Maint ence mode = 6; Blue Led mode = 5;Green Led Mode = 4
const uint8_t VALVE_EEPROM_PROD_ADC_ENDSTOP_24_ADDRESS          = 0x12;
const uint8_t VALVE_EEPROM_PROD_ADC_ENDSTOP_0_ADDRESS           = 0x14;
const uint8_t VALVE_EEPROM_PROD_BACKUP_ADC_ENDSTOP_24_ADDRESS   = 0x28;
const uint8_t VALVE_EEPROM_PROD_BACKUP_ADC_ENDSTOP_0_ADDRESS    = 0x2A;
const uint8_t VALVE_EEPROM_PROD_FIRMWARE_UPDATE_ADDRESS         = 0x41;
const uint8_t VALVE_EEPROM_PROD_DEVICE_DID_ADDRESS              = 0x45;
const uint8_t VALVE_EEPROM_PROD_DEVICE_RID_ADDRESS              = 0x47;

// Pulls EEPROM from memory to cache
void DumpEEPROMtoMemory();

// Gets Valve unique ID from either EEPROM or local cache
void GetUUID(uint8_t *valve_uid);

// Writes EEPROM and Cache with new value at address
void WriteEEPROM(uint8_t addr, uint8_t value);

// Writes EEPROM and Cache with new value of length at address
void WriteEEPROMBuffer(uint8_t eeprom_addr, uint8_t *buffer, uint8_t length);

// Generate Checksum
uint16_t CheckSumMaker(uint8_t *buffer, uint8_t size);

// Reads EEPROM into data at address of size length
void ReadEEPROM(uint8_t *data, uint8_t address, uint8_t length);

#endif	/* EEPROM_CONTROLLER_H */
