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
#include "globals.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h" 
#include "command_system.h"
#include "led_controller.h"
#include "eeprom_controller.h"
#include "settings_state_controller.h"
#include "valve_manager.h"
#include "mcc_generated_files/eusart.h"

/*
                         Main application
 */

void WriteRs485Array(uint8_t * value, uint8_t length);
void ReadButtons(void);

enum
{
    POR         = 0x1,
    ILLEGAL     = 0x2,
    BOR         = 0x3,
    WDT_RESET   = 0x4,
    WDT_WAKE    = 0x5,
    INT_WAKE    = 0x6,
    MCLR        = 0x7,
    MCLR_SLEEP  = 0x8,
    RESET       = 0x9,
    STACK_OVER  = 0xA,
    STACK_UNDER = 0xB,
    BAD_VAL     = 0xC
} Reset_Reason;



const uint8_t first_stop_offset = 1;
const uint8_t first_stop_backup_offset = 32;
const uint8_t second_stop_offset = 3;
const uint8_t second_stop_backup_offset = 34;
const uint8_t mode_offset = 5;
const uint8_t current_position_offset = 8;

const uint8_t ADC_Endstop_24_offset = 18;
const uint8_t ADC_Endstop_0_offset = 20;

const uint8_t firmware_update_mode_offset = 65;


const uint8_t did_offset = 69;
const uint8_t rid_offset = 71;

const uint8_t unused_offset = 0x50;
const uint8_t unused_offset_length = 0x80 - 0x50; // 48 bytes available to us

const uint8_t valve_address_offset = 0x50;

volatile uint8_t eeprom_data[VALVE_EEPROM_SIZE] = {0};

extern volatile bool eusartRxDone;

bool modeButtonPushed = false;
bool saveButtonPushed = false;
bool yellowButtonPushed = false;
bool redButtonPushed = false;



void main(void)
{
    display.raw_leds = 0;
    next_display.raw_leds = display.raw_leds;
    // initialize the device
    SYSTEM_Initialize();
    debugLevel = 0;
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    ADC_SelectChannel(Pot1);
    
    DumpEEPROMtoMemory(eeprom_data);
    // Get the UID from the EEPROM - this should be unique per valve.
    GetUUID(valve_uid);
    
    uint8_t storedValveMode = eeprom_data[mode_offset];
    uint8_t storedValveLocation = eeprom_data[current_position_offset];
    
    ADC_Endstop_24_value = (uint16_t *)&(eeprom_data[ADC_Endstop_24_offset]);
    ADC_Endstop_0_value  = (uint16_t *)&(eeprom_data[ADC_Endstop_0_offset]);
    
    
    SetupValve(storedValveLocation, storedValveMode);
    
    
    SetAddress(eeprom_data[valve_address_offset]);
    SetLeds();
    eeprom_data[mode_offset] = nextValveMode;
    volatile Command * newCommand;
    
    newCommand = GetCommandEntryBuffer();
       
    if (newCommand)
    {
        SetupValveAddressPackets(newCommand, BROADCAST_ADDRESS);
    }
    TransmitMessage(newCommand);
    
    
    newCommand = GetCommandEntryBuffer();
       
    if (newCommand)
    {
        newCommand->protocal = 0x1;
        newCommand->source = GetAddress();
        newCommand->destination = 0x10;
        newCommand->command = (uint8_t)VALVE_STATE;
        newCommand->data[0] = GetCurrentValveMode();
        newCommand->data[1] = GetCurrentValveLocation();
        newCommand->data_length = 2;
    }
    TransmitMessage(newCommand);
            
    ADC_StartConversion();    
    
    while (1)
    {
        CLRWDT();  
        
        if (ADC_IsConversionDone())
        {
            valveADCValue = ADC_GetConversionResult();
            ADC_StartConversion();
        }
        
        if (receiveReady == true)
        {
            ReceiveCommandExecutor();
        }
        
        if (eusartRxDone == true)
        {
            uint8_t * buffer = GetBuffer();
            CopyToUARTRXBuff(buffer, eusartRxCount);
            eusartRxDone = false;
            
        }
        
        if (updateEEPROM == 1)
        {
            eeprom_data[valve_address_offset] = GetAddress();
            uint8_t valve_block[6] = {0};
            valve_block[0] = eeprom_data[valve_address_offset];
            valve_block[1] = eeprom_data[valve_address_offset + 1];
            valve_block[2] = eeprom_data[valve_address_offset + 2];
            valve_block[3] = eeprom_data[valve_address_offset + 3];
            valve_block[4] = eeprom_data[valve_address_offset + 4];
            valve_block[5] = eeprom_data[valve_address_offset + 5];
            WriteEEPROMBuffer(valve_address_offset, valve_block, 6);
            updateEEPROM = 0;
        }
        else if (updateEEPROM == 2)
        {
            eeprom_data[mode_offset] = currentValveMode;
            //WriteEEPROMBuffer(valve_address_offset, valve_block, 6);
            updateEEPROM = 0;
        }
        
        ReadButtons();
        if (modeButtonPushed)
        {
            // If we are in remote mode; move from remote mode but move back to original one
            if ((nextValveMode & VALVE_MODE_REMOTE) == VALVE_MODE_REMOTE)
            {
                nextValveMode -= VALVE_MODE_REMOTE;
            }
            
            nextValveMode++;
            if (nextValveMode > VALVE_MODE_MAINTAINENCE)
            {
                nextValveMode = VALVE_MODE_NORMAL;
            }
            modeButtonPushed = false;
        }
        
        if (nextValveMode != currentValveMode)
        {   
            // If we are in maintaience mode; stay there
            if (IsRemoteEnabled() && (currentValveMode == VALVE_MODE_MAINTAINENCE))
            {
                nextValveMode = VALVE_MODE_MAINTAINENCE;
            } 
            SetLeds();
            eeprom_data[mode_offset] = nextValveMode;
            volatile Command * newCommand;
            newCommand = GetCommandEntryBuffer();

            if (newCommand)
            {
                newCommand->protocal = 0x1;
                newCommand->source = GetAddress();
                newCommand->destination = 0x10;
                newCommand->command = (uint8_t)VALVE_STATE;
                newCommand->data[0] = nextValveMode;
                newCommand->data[1] = GetCurrentValveLocation();
                newCommand->data_length = 2;
            }
            TransmitMessage(newCommand);
            currentValveMode = nextValveMode;
            updateEEPROM = 2;
        }
        if (next_display.raw_leds != display.raw_leds)
        {
            display.raw_leds = next_display.raw_leds;
            ControlLights(&display);
        }   
        
        if (resetValve != 0)
        {
            RESET();
        }
        
        MoveValveToNewPosition();
    }
}

void ReadButtons(void)
{
    bool modeButtonPushedFirst = false;
    bool saveButtonPushedFirst = false;
    bool yellowButtonPushedFirst = false;
    bool redButtonPushedFirst = false;

    modeButtonPushedFirst |= (ModeButton_GetValue() == 0);
    saveButtonPushedFirst |= (SaveButton_GetValue() == 0);
    yellowButtonPushedFirst |= (YellowButton_GetValue() == 0);
    redButtonPushedFirst |= (RedButton_GetValue() == 0);
    
    // TODO - needs to move to a timer
    // Delay for about 50 mS-100mS
    uint8_t delayLoop1 = 0xF0;
    uint8_t delayLoop2 = 0xF0;
    do {
        do {
            delayLoop2 -= 1;
        } while (delayLoop2 != 0);
        delayLoop1 -= 1;
    } while (delayLoop1 != 0);

    if(modeButtonPushedFirst)
    {
        if (ModeButton_GetValue() == 0)
            modeButtonPushed |= true;
        else
            modeButtonPushed = false;
        modeButtonPushedFirst = false;
    }
    if(saveButtonPushedFirst)
    {
        if(SaveButton_GetValue() == 0)
            saveButtonPushed |= true;
        else
            saveButtonPushed = false;
        saveButtonPushedFirst = false;
    }
    if(yellowButtonPushedFirst)
    {
        if(YellowButton_GetValue() == 0)
            yellowButtonPushed |= true;
        else
            yellowButtonPushed = false;
        yellowButtonPushedFirst = false;
    }
    if(redButtonPushedFirst)
    {
        if(RedButton_GetValue() == 0)
            redButtonPushed |= true;
        else
            redButtonPushed = false;
        redButtonPushedFirst = false;
    }
}