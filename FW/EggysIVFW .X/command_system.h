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

#include <xc.h> 
#include "globals.h"
#include "settings_state_controller.h"

typedef enum
{
    VALVE_STATE             = 0x02, // Sent periodically
    VALVE_STATE_CHANGED     = 0x04, // Will Emit when state change finished
    VALVE_DEGREES           = 0x06, // Returns Byte(next position), Byte(0 - moving, 1 not moving), WORD(raw ADC)
    VALVE_DEBUG_DEGREES     = 0x08, // Returns Byte(next position), Byte(0 - moving, 1 not moving), WORD(raw ADC read twice), WORD(neededValue);
    VALVE_STARTING_UP       = 0xE0, // Valve is starting up - Done when 1st VALVE_STATE is sent
    VALVE_RESET_START       = 0xE2, // Valve will emit before resetting if reset command sent
    VALVE_SETTINGS          = 0xE4, // These are the valve settings
    VALVE_SETTINGS_DONE     = 0xE6, // Valve settings have been set and acknowledged
    VALVE_ADDR              = 0xEA, // My address
    VALVE_EEPROM            = 0xEC, // Valve EEPROM Data (For backup)
    VALVE_EEPROM_SET_DONE   = 0xEE  // Valve EEPROM Data (For backup
} rs485_send_commands;

// Commands received by the Valve
typedef enum
{
    //When any change takes place all commands dropped until job finished
    VALVE_GOTO_0_POSITION           = 0x10, // PORT at 0 position is closed
    VALVE_GOTO_24_POSITION          = 0x12, // PORT at 24 position is closed
    VALVE_GOTO_MIDDLE_POSITION      = 0x14, // Move valve to middle position (all open)
    VALVE_SET_DEGREES               = 0x16, // Set a specific value (This between 0 and 0x30, so step size is 180/48=> 3.75)
    VALVE_GET_DEGREES               = 0x17,
    VALVE_SET_DEGREES_FROM_CURRENT  = 0x18,
    
    VALVE_REMOTE_CONTROL            = 0x20, // Valve will enter remote control mode and accept commands if 1st packet is 1; else exit remote mode
            
    VALVE_ENTER_MAINTENACE_MODE     = 0x40, // Valve will move to Yellow Led mode and not respond to remote commands that change state/settings
                                            // !!! No way out remotely - for safety/security, need to exit at pad !!! Can also be set at Pad
                                            // Should still emit VALVE_STATE messages periodically with indication of this mode
    VALVE_DEBUG                     = 0xE0, // Enter a debug mode that outputs lots of data -- Use with caution;
    VALVE_RESET                     = 0xF1, // Forces valve to reset; used for FW Update
    VALVE_FW_UPDATE                 = 0xF3, // Enter FW update on next boot
    VALVE_GET_SETTINGS              = 0xF5, // Returns current valve settings (no state)
    VALVE_SET_SETTINGS              = 0xF6, // Sets valve settings (not state)
    VALVE_GET_ADDR                  = 0xF7, // Gets the valves UUID and address (Part of settings but added shortcut)
    VALVE_SET_ADDR                  = 0xF8, // Sets the valves address, need to pass data as UUID (6 bytes), 0x0, newAddr
    VALVE_GET_EEPROM                = 0xF9, // Gets the EEPROM for backup
    VALVE_SET_EEPROM                = 0xFA, // Use with extreme caution - will overwrite EEPROM      
            
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

extern volatile uint8_t debugLevel;

volatile Command * GetCommandEntryBuffer(void); //Gets an entry in the command buffer or Null if no available
uint8_t TransmitMessage(Command * newCommand); //Adds to transmit buffer if possible; returns true if added, else false; This is non-blocking
void CopyToUARTRXBuff(uint8_t * rx_buffer, uint8_t length);

void ReceiveCommandExecutor(void);

void SetupValveAddressPackets(volatile Command * command, uint8_t destination);