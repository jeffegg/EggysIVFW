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
#include "globals.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h" 
#include "command_system.h"
#include "ledcontroller.h"
#include "eeprom_controller.h"
#include "settings_state_controller.h"
#include "valve_manager.h"
#include "mcc_generated_files/eusart.h"


void WriteRs485Array(uint8_t * value, uint8_t length);
void ReadButtons(void);

extern volatile bool eusartRxDone;

bool modeButtonPushed = false;
bool saveButtonPushed = false;
bool yellowButtonPushed = false;
bool redButtonPushed = false;


extern volatile bool eepromDataValid; // Invalidate eeprom cache
extern volatile uint8_t debugLevel = 0; // Debug l

void main(void)
{
    eepromDataValid = false;
    display.raw_leds = 0;
    next_display.raw_leds = display.raw_leds;
    // initialize the device
    SYSTEM_Initialize();
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    ADC_SelectChannel(Pot1);
    
    // Read EEPROM to local memory
    DumpEEPROMtoMemory();
    // Get Debug Level
    ReadEEPROM(&debugLevel, VALVE_EEPROM_DEBUG_LEVEL_ADDRESS, 1);
    // Get the UID from the EEPROM - this should be unique per valve.
    GetUUID(valve_uid);   
    
    SetupValve(storedValveLocation, storedValveMode);
    
    SetLeds();
    eepromData[mode_offset] = nextValveMode;
    volatile Command * newCommand;  
    
    /*newCommand = GetCommandEntryBuffer();    
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
    TransmitMessage(newCommand);*/
            
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
            eepromData[valve_address_offset] = GetAddress();
            uint8_t valve_block[6] = {0};
            valve_block[0] = eepromData[valve_address_offset];
            valve_block[1] = eepromData[valve_address_offset + 1];
            valve_block[2] = eepromData[valve_address_offset + 2];
            valve_block[3] = eepromData[valve_address_offset + 3];
            valve_block[4] = eepromData[valve_address_offset + 4];
            valve_block[5] = eepromData[valve_address_offset + 5];
            WriteEEPROMBuffer(valve_address_offset, valve_block, 6);
            updateEEPROM = 0;
        }
        else if (updateEEPROM == 2)
        {
            eepromData[mode_offset] = currentValveMode;
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
            eepromData[mode_offset] = nextValveMode;
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

/**
 End of File
*/