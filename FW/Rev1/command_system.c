#include "command_system.h"

#define TX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485TXBuffer [TX_COMMAND_BUFFER_SIZE];

#define RX_COMMAND_BUFFER_SIZE 3 
volatile Command rs485RXBuffer [RX_COMMAND_BUFFER_SIZE];

volatile uint8_t sendInProgress; // Transmit in progress
volatile uint8_t sendBuffersFull; 
volatile uint8_t receiveReady; // Something in the receive buffer that needs to be executed 
volatile uint8_t receiveBuffersFull;
volatile uint8_t receiveBuffersOverflow;

void GetCommandEntryBuffer(Command * newCommand)
{
    
}

uint8_t TransmitMessage(Command * newCommand)
{
    return 0;
}
volatile uint8_t uart_rx_buf_location = 0;
//volatile uint8_t uart_rx_buffer[MAX_DATA_LENGTH] = 0;

void RestartUartRXReceiveBufflocatoin(void)
{
  uart_rx_buf_location = 0;
  return;
}

