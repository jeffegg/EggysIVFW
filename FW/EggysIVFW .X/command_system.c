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
#include "command_system.h"
#include "valve_manager.h"

void WriteRs485Array(uint8_t * value, uint8_t length);
void CommandExecutor(Command *currentRS485RXBuffer);

#define TX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485TXBuffer [TX_COMMAND_BUFFER_SIZE];

#define RX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485RXBuffer [RX_COMMAND_BUFFER_SIZE];

volatile uint8_t sendInProgress; // Transmit in progress
volatile uint8_t sendBuffersFull; 
volatile bool receiveReady; // Something in the receive buffer that needs to be executed 
volatile bool receiveBuffersFull;
volatile bool receiveBuffersOverflow;

extern volatile uint8_t debugLevel = 0;

volatile uint8_t transmitBuffer[MAX_PACKET] = {0};

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

volatile uint8_t uart_rx_buffer[MAX_DATA_LENGTH] = {0};

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
    CommandExecutor(&(rs485RXBuffer[0]));
}

void CommandExecutor(Command *currentRS485RXBuffer)
{
    uint8_t command = currentRS485RXBuffer->command;
    uint8_t source = currentRS485RXBuffer->source;
    Command * newCommand;
    
    if (IsRemoteEnabled() || (command == VALVE_REMOTE_CONTROL))
    {
        switch (command)
        {        
            case VALVE_GOTO_0_POSITION:
                SetNextValveLocation(0x0);             
                receiveReady = false;
                break;
            case VALVE_GOTO_24_POSITION:
                SetNextValveLocation(0x30);             
                receiveReady = false;
                break;
            case VALVE_GOTO_MIDDLE_POSITION:
                SetNextValveLocation(0x18);             
                receiveReady = false;   
                break;  
            case VALVE_SET_DEGREES:
                SetNextValveLocation(currentRS485RXBuffer->data[0]);             
                receiveReady = false;
                break;
            case VALVE_GET_DEGREES:
                newCommand = GetCommandEntryBuffer();
                valveADCValue = ADC_GetConversionResult();
                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetAddress();
                    newCommand->destination = 0x10;
                    newCommand->command = (uint8_t)VALVE_DEGREES;
                    newCommand->data[0] = GetCurrentValveLocation();
                    newCommand->data[1] = 0 ;
                    newCommand->data[2] = (uint16_t)((uint16_t)valveADCValue >> (uint16_t)8) & (uint16_t)0xFF ;
                    newCommand->data[3] = (uint16_t)((uint16_t)valveADCValue >> (uint16_t)0) & (uint16_t)0xFF ;
                    newCommand->data_length = 4;
                    TransmitMessage(newCommand);
                }
                receiveReady = false;
                break;
            case VALVE_REMOTE_CONTROL:
                if(currentRS485RXBuffer->data[0] == 1)
                    nextValveMode |= VALVE_MODE_REMOTE;
                else
                    nextValveMode &= ~VALVE_MODE_REMOTE;
                receiveReady = false;
                break;
            case VALVE_ENTER_MAINTENACE_MODE:
                nextValveMode = VALVE_MODE_MAINTAINENCE;
                receiveReady = false;
                break;

            case VALVE_DEBUG:
                debugLevel = currentRS485RXBuffer->data[0];
                break;
            case VALVE_RESET:
                RESET();
                break;

            case VALVE_GET_ADDR:
                newCommand = GetCommandEntryBuffer();
                if (newCommand)
                {
                    SetupValveAddressPackets(newCommand, source);
                    TransmitMessage(newCommand);
                }
                receiveReady = false;
                break;
            case VALVE_SET_ADDR:
                if (currentRS485RXBuffer->data[0] == valve_uid[0] &&
                        currentRS485RXBuffer->data[1] == valve_uid[1]&&
                        currentRS485RXBuffer->data[2] == valve_uid[2] &&
                        currentRS485RXBuffer->data[3] == valve_uid[3]&& 
                        currentRS485RXBuffer->data[4] == valve_uid[4]&&
                        currentRS485RXBuffer->data[5] == valve_uid[5] &&
                        currentRS485RXBuffer->data[6] == 0)
                {
                    SetAddress(currentRS485RXBuffer->data[7]);  
                    updateEEPROM = 1;
                    newCommand = GetCommandEntryBuffer();
                    if (newCommand)
                    {
                        SetupValveAddressPackets(newCommand, source);
                        TransmitMessage(newCommand);
                    }
                }
                receiveReady = false;
                break;


            default:
                receiveReady = false;
        }
    }
}

void SetupValveAddressPackets(volatile Command * command, uint8_t destination)
{
    if (command)
    {
        command->protocal = 0x1;
        command->source = GetAddress();
        command->destination = destination;
        command->command = (uint8_t)VALVE_ADDR;
        command->data[0] = valve_uid[0];
        command->data[1] = valve_uid[1];
        command->data[2] = valve_uid[2];
        command->data[3] = valve_uid[3];
        command->data[4] = valve_uid[4];
        command->data[5] = valve_uid[5];
        command->data[6] = 0;
        command->data[7] = GetAddress();
        command->data_length = 8;
    }
}
