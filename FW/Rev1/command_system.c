#include "command_system.h"
#include "valve_manager.h"

void WriteRs485Array(uint8_t * value, uint8_t length);
void CommandExecutor(Command *currentRS485RXBuffer);
void SendMessage(Command * command_struct, uint8_t valve_address, uint8_t dest, uint8_t command, uint8_t * data, uint8_t data_length);

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
volatile uint8_t uart_rx_buffer[MAX_DATA_LENGTH] = {0};

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
    
    switch (command)
    {   
        case VALVE_IDENTIFY_UUID:
        case VALVE_IDENTIFY_ADDR:
        case VALVE_SET_ADDR:
        case VALVE_GET_ADDR:
        case VALVE_GET_UUID:
        case VALVE_SET_ENDSTOPS:
        case VALVE_GET_ENDSTOPS:
        case VALVE_SET_SETTINGS:
        case VALVE_GET_SETTINGS:
        case VALVE_GET_EEPROM:
        case VALVE_SET_EEPROM:
        case VALVE_GET_MODE:
        case VALVE_SET_MODE:
            break;
        case VALVE_DEBUG:
            debugLevel = currentRS485RXBuffer->data[0];
            break;
        case VALVE_RESET:
            RESET();
            break;
        case VALVE_FW_UPDATE:
            break;
        /*
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
            break;*/


        default:
            receiveReady = false;
    }
}

void SendMessage(volatile Command * command_struct, uint8_t valve_address, uint8_t dest, uint8_t command, uint8_t * data, uint8_t data_length)
{
    if (command)
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
    SendMessage(command, valve_address, 0xF, VALVE_HAIL_MESSAGE, valve_uid, 6);
}

void SendValveAddress(volatile Command * command, uint8_t valve_address)
{
    SendMessage(command, valve_address, 0xF, VALVE_ADDR, valve_address, 1);
}

void SendValveUUID(volatile Command * command, uint8_t valve_address, uint8_t* valve_uid)
{
    SendMessage(command, valve_address, 0xF, VALVE_UUID, valve_uid, 6);
}