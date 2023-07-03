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
#include <stdbool.h>
#include "command_system.h"
#include "valve_manager.h"
#include "settings_state_controller.h"
#include "ledcontroller.h"
#include "version.h"

void WriteRs485Array(uint8_t * value, uint8_t length);

void SendMessage(volatile Command * command_struct, uint8_t valve_address, uint8_t dest, uint8_t command, uint8_t * data, uint8_t data_length);

#define TX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485TXBuffer [TX_COMMAND_BUFFER_SIZE];

#define RX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485RXBuffer [RX_COMMAND_BUFFER_SIZE];

volatile uint8_t sendInProgress; // Transmit in progress
volatile uint8_t sendBuffersFull; 
volatile bool receiveReady; // Something in the receive buffer that needs to be executed 
volatile bool receiveBuffersFull;
volatile bool receiveBuffersOverflow;


extern volatile bool resetValve = false;
extern volatile bool fwUpdateValue = false;
volatile uint8_t uart_rx_buffer[MAX_DATA_LENGTH] = {0};

volatile uint8_t transmitBuffer[MAX_PACKET] = {0};
void (*CommandExecutor)(volatile Command *);

void UnprovisionedCommandExecutor(volatile Command *currentRS485RXBuffer);
void ProvisionedCommandExecutor(volatile Command *currentRS485RXBuffer);

extern uint8_t sendEEPROMAddress = 0;
extern uint8_t sendEEPROMLength = 0;
extern bool sendEEPROM = false;
extern uint8_t sendEEPROMSource = 0;

extern bool writeEEPROM = false;
extern uint8_t writeEEPROMSource = 0;
extern uint8_t writeEEPROMAddress = 0;
extern uint8_t writeEEPROMLength = 0;
extern uint32_t writeEEPROMValue = 0;

uint8_t *fw_version;
uint8_t *fw_date;
uint8_t *deviceID;
uint8_t *revisionID;

void SetupCommandManager(void)
{
    if(IsProvisioned())
        CommandExecutor = &ProvisionedCommandExecutor;
    else
        CommandExecutor = &UnprovisionedCommandExecutor;
}


volatile Command * GetCommandEntryBuffer(void)
{    
    return &rs485TXBuffer[0];
}

uint8_t TransmitMessage(Command * newCommand)
{
    transmitBuffer[0] = 0xFF;
    transmitBuffer[1] = 0x00;
    transmitBuffer[2] = 0xFF;
    transmitBuffer[3] = 0xA5;
    transmitBuffer[4] = newCommand->protocal;
    transmitBuffer[5] = newCommand->destination;
    transmitBuffer[6] = newCommand->source;
    transmitBuffer[7] = newCommand->command;
    transmitBuffer[8] = newCommand->data_length;
    for (uint8_t i = 0; i < newCommand->data_length; ++i)
    {
        transmitBuffer[9 + i] = newCommand->data[i];
    }
    uint16_t checksum = 0;
    uint8_t checkSumLocation = 9 + newCommand->data_length;
    
    for (uint8_t i = 3; i < checkSumLocation; ++i)
    {
        checksum += (uint16_t)transmitBuffer[i];
    }
    transmitBuffer[checkSumLocation] = (checksum >> (uint16_t)0x8) & (uint16_t)0xFF;
    transmitBuffer[checkSumLocation + 1] = checksum & 0xFF;
    
    WriteRs485Array(transmitBuffer, checkSumLocation+2);
    
    return 0;
}

void CopyToUARTRXBuff(uint8_t * rx_buffer, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        uart_rx_buffer[i] = rx_buffer[i];
    }
    
    receiveReady = true;
}

void WriteRs485Array(uint8_t * value, uint8_t length)
{
    uint8_t i = 0;
    DE_SetHigh();
    nRE_SetHigh(); 
    while(i < length){
        CLRWDT();
        if(EUSART_is_tx_ready())
        {    
            EUSART_Write(value[i]);
            i++;
        }
    }
    while (!EUSART_is_tx_done())
    {
        CLRWDT();
    }
    
    DE_SetLow();
    nRE_SetLow();
}

void ReceiveCommandExecutor(void)
{
    rs485RXBuffer[0].protocal = uart_rx_buffer[4];
    rs485RXBuffer[0].destination = uart_rx_buffer[5];
    rs485RXBuffer[0].source = uart_rx_buffer[6];
    rs485RXBuffer[0].command = uart_rx_buffer[7];
    rs485RXBuffer[0].data_length = uart_rx_buffer[8];
    
    for (uint8_t i =0; i < uart_rx_buffer[8]; i++)
    {
        rs485RXBuffer[0].data[i] = uart_rx_buffer[9 + i];
    }
    
    volatile Command * currentBuffer = &rs485RXBuffer[0];
    (*CommandExecutor)(currentBuffer);
}

void UnprovisionedCommandExecutor(volatile Command *currentRS485RXBuffer)
{
    uint8_t command = currentRS485RXBuffer->command;
    uint8_t source = currentRS485RXBuffer->source;
    Command * newCommand;
    
    switch (command)
    {   
        case VALVE_IDENTIFY_UUID:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                IdentifyValve();
            }
            receiveReady = false;
            break;
        case VALVE_SET_ADDR:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                uint8_t valve_address = currentRS485RXBuffer->data[6];
                SetValveRs485Address(valve_address);
                ProvisionValve(true); 
                CommandExecutor = &ProvisionedCommandExecutor;
            }
            receiveReady = false;
            break;
        case VALVE_GET_ADDR:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                newCommand = GetCommandEntryBuffer();
                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetValveRs485Address();
                    newCommand->destination = currentRS485RXBuffer->source;
                    newCommand->command = (uint8_t)VALVE_ADDR;
                    newCommand->data[0] = GetValveRs485Address();
                    newCommand->data_length = 1;
                    TransmitMessage(newCommand);
                }
                receiveReady = false;
            }
            receiveReady = false;
            break;
        case VALVE_SET_SETTINGS:
        case VALVE_GET_SETTINGS:
        case VALVE_GET_EEPROM:
        case VALVE_SET_EEPROM:
            receiveReady = false;
            break;
        case VALVE_DEBUG:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                uint8_t debugLevel = currentRS485RXBuffer->data[6];
                SetDebugLevel(debugLevel);
            }
            receiveReady = false;
            break;
        case VALVE_RESET:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                resetValve = currentRS485RXBuffer->data[6] == 1;
            }
            receiveReady = false;
            break;
        case VALVE_FW_UPDATE:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                fwUpdateValue = currentRS485RXBuffer->data[6] == 1;
            }
            receiveReady = false;
            break;
        case VALVE_FW_VERISON:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                newCommand = GetCommandEntryBuffer();
                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetValveRs485Address();
                    newCommand->destination = currentRS485RXBuffer->source;
                    newCommand->command = (uint8_t)VALVE_FW_VERISON;
                    newCommand->data[0] = fw_version[0];
                    newCommand->data[1] = fw_version[1];
                    newCommand->data[2] = fw_version[2];
                    newCommand->data[3] = fw_version[3];
                    newCommand->data[4] = 0x0;
                    newCommand->data[5] = fw_date[0];
                    newCommand->data[6] = fw_date[1];
                    newCommand->data[7] = fw_date[2];
                    newCommand->data[8] = fw_date[3];
                    newCommand->data[9] = 0x1;
                    newCommand->data[10] = deviceID[0];
                    newCommand->data[11] = deviceID[1];
                    newCommand->data[12] = 0x2;
                    newCommand->data[13] = revisionID[2];
                    newCommand->data[14] = revisionID[3];  
                    
                    newCommand->data_length = 15;
                    TransmitMessage(newCommand);
                }
                receiveReady = false;
            }
            receiveReady = false;
            break;
        default:
            receiveReady = false;
            break;
    }
}

void ProvisionedCommandExecutor(volatile Command *currentRS485RXBuffer)
{
    uint8_t debugLevel = 0;
    uint8_t command = currentRS485RXBuffer->command;
    uint8_t source = currentRS485RXBuffer->source;
    Command * newCommand;
    
    switch (command)
    {   
        case VALVE_IDENTIFY_UUID:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                IdentifyValve();
            }
            receiveReady = false;
            break;
        case VALVE_SET_ADDR:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                uint8_t valve_address = currentRS485RXBuffer->data[6];
                SetValveRs485Address(valve_address);
                if (valve_address == 0xB3)
                    ProvisionValve(false); 
                else
                    ProvisionValve(true); 
                CommandExecutor = &ProvisionedCommandExecutor;
            }
            receiveReady = false;
            break;
        case VALVE_GET_ADDR:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                newCommand = GetCommandEntryBuffer();
                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetValveRs485Address();
                    newCommand->destination = currentRS485RXBuffer->source;
                    newCommand->command = (uint8_t)VALVE_ADDR;
                    newCommand->data[0] = GetValveRs485Address();
                    newCommand->data_length = 1;
                    TransmitMessage(newCommand);
                }
                receiveReady = false;
            }
            receiveReady = false;
            break;
        case VALVE_SET_SETTINGS:
            receiveReady = false;
            break;
        case VALVE_GET_SETTINGS:
            receiveReady = false;
            break;
        case VALVE_GET_EEPROM:
            sendEEPROMAddress = currentRS485RXBuffer->data[0];
            sendEEPROMLength = currentRS485RXBuffer->data[1];
            sendEEPROMSource = source;
            sendEEPROM = true;
            receiveReady = false;
            break;
        case VALVE_SET_EEPROM:
            writeEEPROMSource = source;
            writeEEPROMAddress = currentRS485RXBuffer->data[0];
            writeEEPROMLength = currentRS485RXBuffer->data[1];
            for (uint8_t i = 0; i < writeEEPROMLength; i++)
                ((uint8_t *)&writeEEPROMValue)[i] = currentRS485RXBuffer->data[2 + i];
            writeEEPROM = true;
            receiveReady = false;
            break;
        case VALVE_DEBUG:
            debugLevel = currentRS485RXBuffer->data[6];
            SetDebugLevel(debugLevel);
            receiveReady = false;
            break;
        case VALVE_RESET:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                resetValve = currentRS485RXBuffer->data[6] == 1;
            }
            receiveReady = false;
            break;
        case VALVE_FW_UPDATE:
            if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                    currentRS485RXBuffer->data[1] == valve_uid[1]&&
                    currentRS485RXBuffer->data[2] == valve_uid[2] &&
                    currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                    currentRS485RXBuffer->data[4] == valve_uid[4]&&
                    currentRS485RXBuffer->data[5] == valve_uid[5])
            {
                fwUpdateValue = currentRS485RXBuffer->data[6] == 1;
            }
            receiveReady = false;
            break;
        case VALVE_FW_VERISON:
            newCommand = GetCommandEntryBuffer();
            if (newCommand)
            {
                newCommand->protocal = 0x1;
                newCommand->source = GetValveRs485Address();
                newCommand->destination = currentRS485RXBuffer->source;
                newCommand->command = (uint8_t)VALVE_FW_VERISON;
                newCommand->data[0] = fw_version[0];
                newCommand->data[1] = fw_version[1];
                newCommand->data[2] = fw_version[2];
                newCommand->data[3] = fw_version[3];
                newCommand->data[4] = 0x0;
                newCommand->data[5] = fw_date[0];
                newCommand->data[6] = fw_date[1];
                newCommand->data[7] = fw_date[2];
                newCommand->data[8] = fw_date[3];
                newCommand->data[9] = 0x1;
                newCommand->data[10] = deviceID[0];
                newCommand->data[11] = deviceID[1];
                newCommand->data[12] = 0x2;
                newCommand->data[13] = revisionID[2];
                newCommand->data[14] = revisionID[3];  

                newCommand->data_length = 15;
                TransmitMessage(newCommand);
            }
            receiveReady = false;
            break;
        default:
            receiveReady = false;
            break;
    }
}

void SendMessage(volatile Command * command_struct, uint8_t valve_address, uint8_t dest, uint8_t command, uint8_t * data, uint8_t data_length)
{
    if (command_struct)
    {
        command_struct->protocal = 0x1;
        command_struct->source = valve_address;
        command_struct->destination = dest;
        command_struct->command = command;
        for (uint8_t i = 0; i < data_length; i++)
        {
            command_struct->data[i] = data[i];
        }
        command_struct->data_length = data_length;
    }
}

void SendValveHailMessage(volatile Command * command, uint8_t valve_address, uint8_t* valve_uid)
{
    uint8_t newData[VALVE_EEPROM_SERIAL_LEN + 14 + 1] = {0};
    
    for (int i = 0; i < VALVE_EEPROM_SERIAL_LEN; i++)
        newData[i] =  valve_uid[i];
       
    newData[VALVE_EEPROM_SERIAL_LEN + 0] = fw_version[0];
    newData[VALVE_EEPROM_SERIAL_LEN + 1] = fw_version[1];
    newData[VALVE_EEPROM_SERIAL_LEN + 2] = fw_version[2];
    newData[VALVE_EEPROM_SERIAL_LEN + 3] = fw_version[3];
    newData[VALVE_EEPROM_SERIAL_LEN + 4] = 0x0;
    newData[VALVE_EEPROM_SERIAL_LEN + 5] = fw_date[0];
    newData[VALVE_EEPROM_SERIAL_LEN + 6] = fw_date[1];
    newData[VALVE_EEPROM_SERIAL_LEN + 7] = fw_date[2];
    newData[VALVE_EEPROM_SERIAL_LEN + 8] = fw_date[3];
    newData[VALVE_EEPROM_SERIAL_LEN + 9] = 0x1;
    newData[VALVE_EEPROM_SERIAL_LEN + 10] = deviceID[0];
    newData[VALVE_EEPROM_SERIAL_LEN + 11] = deviceID[1];
    newData[VALVE_EEPROM_SERIAL_LEN + 12] = 0x2;
    newData[VALVE_EEPROM_SERIAL_LEN + 13] = revisionID[2];
    newData[VALVE_EEPROM_SERIAL_LEN + 14] = revisionID[3];  
    
    SendMessage(command, valve_address, 0xF, VALVE_HAIL_MESSAGE, newData, VALVE_EEPROM_SERIAL_LEN + 14 + 1);
}

void SendValveAddress(volatile Command * command, uint8_t valve_address)
{
    SendMessage(command, valve_address, 0xF, VALVE_ADDR, &valve_address, 1);
}

void SendValveUUID(volatile Command * command, uint8_t valve_address, uint8_t* valve_uid)
{
    SendMessage(command, valve_address, 0xF, VALVE_UUID, valve_uid, 6);
}