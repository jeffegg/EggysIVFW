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
#include "eeprom_controller.h"
#include "mcc_generated_files/eusart.h"

void WriteRs485Array(uint8_t * value, uint8_t length);

extern volatile bool eusartRxDone;

void main(void)
{
    SYSTEM_Initialize();
    SetupEEPROM();
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Read EEPROM to local memory
    DumpEEPROMtoMemory();
    SetupCommandManager();   
    
    volatile Command * newCommand;  
        
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

        PeriodicEEPROM();

    }
}
/**
 End of File
*/