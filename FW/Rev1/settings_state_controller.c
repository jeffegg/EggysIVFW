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
#include "mcc_generated_files/tmr0.h"
#include "command_system.h"

void GetUUIDFromEEPROM(void);
void GetDID_RIDFromEEPROM(void);


volatile uint16_t localDeviceID = 0;
volatile uint16_t localRevisionID = 0;
extern uint8_t valve_uid[VALVE_EEPROM_SERIAL_LEN] = {0};
uint8_t nextDebugLevel = 0;
uint8_t currentDebugLevel = 0;
uint8_t nextValveAddress = 0xB3;
uint8_t currentValveAddress = 0;
volatile bool nextValveProvisioned = false;
volatile bool currentValveProvisioned = false;

volatile uint8_t missedProvisionedCount = 0;
volatile uint8_t sendValveHailCount = 0;

extern volatile bool provisonSeen = false;

extern uint8_t *deviceID = (uint8_t *)&localDeviceID;
extern uint8_t *revisionID = (uint8_t *)&localRevisionID;

extern volatile bool sendValveID = false;
volatile bool sendValveHail = 0;

void LoadValveSettings(void)
{
    missedProvisionedCount = 0;
    sendValveHailCount = 0;
    
    GetUUIDFromEEPROM();
    GetDID_RIDFromEEPROM();
    
    ReadEEPROM(&nextDebugLevel, VALVE_EEPROM_DEBUG_LEVEL_ADDRESS, 1);
    if (nextDebugLevel == 0xFF)
    {
        nextDebugLevel = 0;
        WriteEEPROM(VALVE_EEPROM_DEBUG_LEVEL_ADDRESS, nextDebugLevel);
    }
    currentDebugLevel = nextDebugLevel;
    
    ReadEEPROM(&nextValveAddress, VALVE_EEPROM_RS485_ADDRESS, 1);
    if (nextValveAddress == 0xFF)
    {
        nextValveAddress = 0xB3;
        WriteEEPROM(VALVE_EEPROM_RS485_ADDRESS, nextValveAddress);
    }
    currentValveAddress = nextValveAddress;
    
    // I don't think we should load... If not we will wait 3 minutes to see.
    /*uint8_t tempValue = 0;
    ReadEEPROM(&tempValue, VALVE_EEPROM_PROVISONED_ADDRESS, 1);
    if (tempValue == 0xFF)
    {
        nextValveProvisioned = false;
        WriteEEPROM(VALVE_EEPROM_PROVISONED_ADDRESS, (uint8_t)nextValveProvisioned);
    }  
    else
    {
        nextValveProvisioned = (bool)tempValue;
    }*/
    nextValveProvisioned = false;
    currentValveProvisioned = nextValveProvisioned;
    TMR0_SetProvisionedInterruptHandler(ProvisionedTimeFunction);
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



void SetDebugLevel(uint8_t value)
{
    nextDebugLevel = value;
}

uint8_t GetDebugLevel(void)
{
    return currentDebugLevel;
}

void ProvisionValve(bool provisioned)
{
    nextValveProvisioned = provisioned;
}

inline bool IsProvisioned(void)
{
    return currentValveProvisioned;
}

void ProvisionedTimeFunction(void)
{
    if (!provisonSeen)
    {
        missedProvisionedCount++;
    }
    else
    {
        missedProvisionedCount = 0;
        nextValveProvisioned = true;
    }
    if (missedProvisionedCount >= 12) // No commands seen 10 seconds * 12 = 2 minutes
    {
        // If valve is provisioned then unprovision it
        if (currentValveProvisioned)
        {
            nextValveProvisioned = false;
        }
        missedProvisionedCount = 0;
    }
    provisonSeen = false;
    if (sendValveHailCount > 12) // Send 12 at about 10s ->> about 2 minutes
    {
        sendValveHailCount = 0;
        sendValveID = false;
    }
    if (sendValveID) {
        sendValveHail = true;
        sendValveHailCount++;
    }
}

void SettingsManagerRun(void)
{    
    if (nextDebugLevel != currentDebugLevel)
    {
        WriteEEPROM(VALVE_EEPROM_DEBUG_LEVEL_ADDRESS, nextDebugLevel);
        currentDebugLevel = nextDebugLevel;
    }
    
    if (nextValveAddress != currentValveAddress)
    {
        WriteEEPROM(VALVE_EEPROM_RS485_ADDRESS, nextValveAddress);
        currentValveAddress = nextValveAddress;
    }
            
    if (nextValveProvisioned != currentValveProvisioned)
    {
        //WriteEEPROM(VALVE_EEPROM_PROVISONED_ADDRESS, (uint8_t)nextValveProvisioned);
        currentValveProvisioned = nextValveProvisioned;
        ProvisionValve(currentValveProvisioned);
    }
    if (sendValveHail)
    {
        volatile Command * newCommand;          
        newCommand = GetCommandEntryBuffer();    
        if (newCommand)
        {
            SendValveHailMessage(newCommand, GetValveRs485Address());
        }
        TransmitMessage(newCommand);  
        sendValveHail = false;
    }
        
}
