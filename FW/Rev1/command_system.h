/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef COMMAND_SYSTEM_H
#define	COMMAND_SYSTEM_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Commands sent by the Valve

// Lazy way, don't worry on the malloc; just set fixed sizes
#define MAX_DATA_LENGTH 0x50
#define MAX_PACKET (3 + 1 + 1 + 1 +1 +1 + MAX_DATA_LENGTH + 2)

typedef enum
{
    VALVE_STATE             = 0x02, // Sent periodically
    VALVE_STATE_CHANGED     = 0x04, // Will Emit when state change finished
    
    VALVE_STARTING_UP       = 0xE0, // Valve is starting up - Done when 1st VALVE_STATE is sent
    VALVE_RESET_START       = 0xE2, // Valve will emit before resetting if reset command sent
    VALVE_SETTINGS          = 0xE4, // These are the valve settings
    VALVE_SETTINGS_DONE     = 0xE6, // Valve settings have been set and acknowledged
    VALVE_NAME              = 0xEA, // My Name
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
    VALVE_SET_DEGREES_FROM_CURRENT  = 0x18,
    
    VALVE_ENTER_MAINTENACE_MODE     = 0x40, // Valve will move to Yellow Led mode and not respond to remote commands that change state/settings
                                            // !!! No way out remotely - for safety/security, need to exit at pad !!! Can also be set at Pad
                                            // Should still emit VALVE_STATE messages periodically with indication of this mode
    
    VALVE_RESET                     = 0xF1, // Forces valve to reset; used for FW Update
    VALVE_FW_UPDATE                 = 0xF3, // Enter FW update on next boot
    VALVE_GET_SETTINGS              = 0xF5, // Returns current valve settings (no state)
    VALVE_SET_SETTINGS              = 0xF7, // Sets valve settings (not state)
    VALVE_GET_NAME                  = 0xF9, // Gets the values Name (Part of settings but added shortcut)
    VALVE_GET_EEPROM                = 0xFB, // Gets the EEPROM for backup
    VALVE_SET_EEPROM                = 0xFD, // Use with extreme caution - will overwrite EEPROM      
            
} rs485_receive_commands;


typedef struct 
{   
    //Header will be added automatically => 0x00 0xFF 0xA5
    // Max size is 3 + 1 + 1 + 1 +1 +1 + MAX_DATA_LENGTH + 2 => 0x5A
    uint8_t header0;
    uint8_t header1;
    uint8_t header2;
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
extern volatile uint8_t receiveReady; // Something in the receive buffer that needs to be executed 
extern volatile uint8_t receiveBuffersFull;
extern volatile uint8_t receiveBuffersOverflow;

void GetCommandEntryBuffer(Command * newCommand); //Gets an entry in the command buffer or Null if no available
uint8_t TransmitMessage(Command * newCommand); //Adds to transmit buffer if possible; returns true if added, else false; This is non-blocking

void ReceiveCommandExecutor();

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* COMMAND_SYSTEM_H */

