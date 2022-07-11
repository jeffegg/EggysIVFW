#include "command_system.h"

void WriteRs485Array(uint8_t * value, uint8_t length);

#define TX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485TXBuffer [TX_COMMAND_BUFFER_SIZE];

#define RX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485RXBuffer [RX_COMMAND_BUFFER_SIZE];

volatile uint8_t sendInProgress; // Transmit in progress
volatile uint8_t sendBuffersFull; 
volatile uint8_t receiveReady; // Something in the receive buffer that needs to be executed 
volatile uint8_t receiveBuffersFull;
volatile uint8_t receiveBuffersOverflow;

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
    
    for (uint8_t i = 4; i < checkSumLocation; ++i)
    {
        checksum += transmitBuffer[i];
    }
    transmitBuffer[checkSumLocation] = (checksum & 0xFF) >> 0x8;
    transmitBuffer[checkSumLocation + 1] = checksum & 0xFF;
    
    WriteRs485Array(transmitBuffer, checkSumLocation+2);
    
    return 0;
}
volatile uint8_t uart_rx_buf_location = 0;
//volatile uint8_t uart_rx_buffer[MAX_DATA_LENGTH] = 0;

void RestartUartRXReceiveBufflocatoin(void)
{
  uart_rx_buf_location = 0;
  return;
}

void WriteRs485Array(uint8_t * value, uint8_t length)
{
    uint8_t i = 0;
    DE_SetHigh();
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
    
}

