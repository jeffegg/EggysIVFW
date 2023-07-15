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

#ifndef COMMAND_SYSTEM_H
#define	COMMAND_SYSTEM_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include "globals.h"
#include "settings_state_controller.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h"

typedef enum
{
    VALVE_EEPROM            = 0x39, 
    VALVE_EEPROM_SET_DONE   = 0x3B,  // Valve EEPROM Data (For backup
    VALVE_FW_VERISON        = 0xF9
} rs485_send_commands;

// Commands received by the Valve
typedef enum
{
    VALVE_GET_EEPROM        = 0x38, // Gets the EEPROM for backup
    VALVE_SET_EEPROM        = 0x3A, // Use with extreme caution - will overwrite EEPROM           
    VALVE_RESET             = 0xF1, // Forces valve to reset; used for FW Update
    VALVE_FW_UPDATE         = 0xF3, // Enter FW update on next boot   
    VALVE_GET_FW_VERSION    = 0xF8  // Get the FW version, Device ID, and Revison ID of the PIC Device
} rs485_receive_commands;


typedef struct 
{   
    //Header will be added automatically => 0xFF 0x00 0xFF 0xA5
    // Max size is 4 + 1 + 1 + 1 +1 +1 + MAX_DATA_LENGTH + 2 => 0x5A
    /*uint8_t header0;
    uint8_t header1;
    uint8_t header2;*/
    uint8_t protocal;
    uint8_t destination;
    uint8_t source;
    uint8_t command;
    uint8_t data_length;
    uint8_t data[MAX_DATA_LENGTH]; // Fix the size and skip the malloc
    uint16_t checksum;
}Command;

extern volatile uint8_t sendInProgress; // Transmit in progress
extern volatile uint8_t sendBuffersFull; 
extern volatile bool receiveReady; // Something in the receive buffer that needs to be executed 
extern volatile bool receiveBuffersFull;
extern volatile bool receiveBuffersOverflow;

extern volatile bool resetValve;
extern volatile bool fwUpdateValue;
void SetupCommandManager(void);

uint8_t TransmitMessage(Command * newCommand); //Adds to transmit buffer if possible; returns true if added, else false; This is non-blocking

volatile Command * GetCommandEntryBuffer(void); //Gets an entry in the command buffer or Null if no available
void CopyToUARTRXBuff(uint8_t * rx_buffer, uint8_t length);
void ReceiveCommandExecutor(void);

#endif	/* COMMAND_SYSTEM_H */

