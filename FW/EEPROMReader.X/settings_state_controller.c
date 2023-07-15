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

#include <xc.h> // include processor files - each processor file is guarded. 
#include "globals.h"
#include "settings_state_controller.h"
#include "eeprom_controller.h"

void GetUUIDFromEEPROM(void);
void GetDID_RIDFromEEPROM(void);

volatile uint16_t localDeviceID = 0;
volatile uint16_t localRevisionID = 0;
uint8_t valve_uid[VALVE_EEPROM_SERIAL_LEN] = {0};
uint8_t nextValveAddress = 0xB3;
uint8_t currentValveAddress = 0;

extern uint8_t *deviceID = (uint8_t *)&localDeviceID;
extern uint8_t *revisionID = (uint8_t *)&localRevisionID;

void LoadValveSettings(void)
{
    
    GetUUIDFromEEPROM();
    GetDID_RIDFromEEPROM();
    
    ReadEEPROM(&nextValveAddress, VALVE_EEPROM_RS485_ADDRESS, 1);
    if (nextValveAddress == 0xFF)
    {
        nextValveAddress = 0xB3;
    }
    currentValveAddress = nextValveAddress;
}

void SetValveRs485Address(uint8_t newAddress)
{
    nextValveAddress = newAddress;
}

uint8_t GetValveRs485Address(void)
{
    return currentValveAddress;
}

void GetUUIDFromEEPROM(void)
{
    ReadEEPROM(valve_uid, VALVE_EEPROM_UUID_ADDRESS, VALVE_EEPROM_SERIAL_LEN);
}

void GetDID_RIDFromEEPROM(void)
{
    ReadEEPROM((uint8_t *)&localDeviceID, VALVE_EEPROM_PROD_DEVICE_DID_ADDRESS, 2);
    ReadEEPROM((uint8_t *)&localRevisionID, VALVE_EEPROM_PROD_DEVICE_RID_ADDRESS, 2);
}

void SettingsManagerRun(void)
{    
}
