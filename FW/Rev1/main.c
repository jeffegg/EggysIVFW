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
#include "button_manager.h"
#include "mcc_generated_files/eusart.h"


void WriteRs485Array(uint8_t * value, uint8_t length);
void ResetInfo(void);

extern volatile bool eusartRxDone;
extern volatile bool sendPeriodicEndStop;

extern uint8_t reset_reason = 0;
extern uint8_t reset_pcon = 0;
extern uint8_t status = 0;

void main(void)
{
    ResetInfo();
    SYSTEM_Initialize();
    SetupEEPROM();
    SetupLeds();
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    ADC_SelectChannel(Pot1);
    
    // Read EEPROM to local memory
    DumpEEPROMtoMemory();
    LoadValveSettings();
    SetupValve(); 
    SetLeds();
    UpdateLeds();
    SetupCommandManager();   
    ButtonManagerSetup();
    
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
    
    while (1)
    {
        CLRWDT();         
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
               
        ReadButtons();
        if(fwUpdateValue)
        {
            WriteEEPROM(VALVE_EEPROM_PROD_FIRMWARE_UPDATE_ADDRESS, 0xFE);  
        }
        
        if (resetValve)
        {
            RESET();
            NOP();
            NOP();
            NOP();
            NOP();
        }
        SetLeds();
        UpdateLeds();
        CLRWDT(); 
        PeriodicValveUpdate();
        SettingsManagerRun();
        PeriodicEEPROM();
        if (sendPeriodicEndStop)
        {
            SendValveENDStop(0x0);
            sendPeriodicEndStop = false;
        }
    }
}

void ResetInfo(void)
{
    reset_pcon = PCON;
    status = (__timeout << 4) | (__powerdown << 3);
    
    if (PCONbits.STKOVF == 1)
    {
        reset_reason = 1;
    }
    else if (PCONbits.STKUNF == 1)
    {
        reset_reason = 2;
    }
    else if ((PCONbits.STKOVF == 0) && (PCONbits.STKUNF == 0) && (PCONbits.nRWDT == 1) && (PCONbits.nRMCLR == 1) && (PCONbits.nRI == 1) && (PCONbits.nBOR == 0) && (__timeout == 1) && (__powerdown == 1))
    {
        reset_reason = 3; //"Power-on Reset";
    }
    else if ((PCONbits.STKOVF == 0) && (PCONbits.STKUNF == 0) && (PCONbits.nRWDT == 1) && (PCONbits.nRMCLR == 1) && (PCONbits.nRI == 1) && (PCONbits.nBOR == 0) && (__timeout == 0))
    {
        reset_reason = 4; //"Illegal, TO is set on POR";
    }
    else if ((PCONbits.STKOVF == 0) && (PCONbits.STKUNF == 0) && (PCONbits.nRWDT == 1) && (PCONbits.nRMCLR == 1) && (PCONbits.nRI == 1) && (PCONbits.nBOR == 0) && (__powerdown == 0))
    {
        reset_reason = 5; //"Illegal, PD is set on POR";
    }
    else if ((PCONbits.STKOVF == 0) && (PCONbits.STKUNF == 0) && (PCONbits.nRMCLR == 1) && (PCONbits.nRI == 1) && (PCONbits.nBOR == 0) && (__powerdown == 0) && (__timeout == 1) && (__powerdown == 1))
    {
        reset_reason = 6; //"Brown-out Reset";
    }
    else if ((PCONbits.nRWDT == 0) && (__timeout == 0))
    {
        reset_reason = 7; //"WDT Reset ";
    }
    else if ((__timeout == 0) && (__powerdown == 0))
    {
        reset_reason = 8; //"WDT Wake-up from Sleep";
    }
    else if ((PCONbits.nRMCLR == 0) && (__timeout == 1) && (__powerdown == 0))
    {
        reset_reason = 9; //"MCLR Reset during Sleep";
    }
    else if (__timeout == 1)
    {
        reset_reason =0xA; //"Interrupt Wake-up from Sleep";
    }
    else if (PCONbits.nRMCLR == 0)
    {
        reset_reason = 0xB; //"MCLR Reset during normal operation";
    }
    else if (PCONbits.nRI == 0)
    {
        reset_reason = 0xC; //"RESET Instruction Executed";
    }
}

/**
 End of File
*/