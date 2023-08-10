/**
  EUSART Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart.c

  @Summary
    This is the generated driver implementation file for the EUSART driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for EUSART.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1718
        Driver Version    :  2.1.1
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/
#include "eusart.h"
#include "../globals.h"
#include "../settings_state_controller.h"
/**
  Section: Macro Declarations
*/



/**
  Section: Global Variables
*/
volatile uint8_t eusartTxHead = 0;
volatile uint8_t eusartTxTail = 0;
volatile uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
volatile uint8_t eusartTxBufferRemaining;

volatile uint8_t eusartRxHead = 0;
volatile uint8_t eusartRxTail = 0;
volatile uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile eusart_status_t eusartRxStatusBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;
volatile uint16_t eusartRxChecksum = 0;
volatile uint8_t eusartRxChecksumLoc = 0;
volatile uint16_t eusartCalcChecksum = 0;
volatile int8_t eusartRxLength = 0;
volatile eusart_status_t eusartRxLastError;
volatile bool eusartRxDone = false;

volatile uint8_t temp_reg = 0;

/**
  Section: EUSART APIs
*/
void (*EUSART_TxDefaultInterruptHandler)(void);
void (*EUSART_RxDefaultInterruptHandler)(void);

void (*EUSART_FramingErrorHandler)(void);
void (*EUSART_OverrunErrorHandler)(void);
void (*EUSART_ErrorHandler)(void);

void EUSART_DefaultFramingErrorHandler(void);
void EUSART_DefaultOverrunErrorHandler(void);
void EUSART_DefaultErrorHandler(void);

void EUSART_Initialize(void)
{
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    EUSART_SetRxInterruptHandler(EUSART_Receive_ISR);
    PIE1bits.TXIE = 0;
    EUSART_SetTxInterruptHandler(EUSART_Transmit_ISR);
    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x24;

    // SP1BRGL 64; 
    SP1BRGL = 0x40;

    // SP1BRGH 3; 
    SP1BRGH = 0x03;


    EUSART_SetFramingErrorHandler(EUSART_DefaultFramingErrorHandler);
    EUSART_SetOverrunErrorHandler(EUSART_DefaultOverrunErrorHandler);
    EUSART_SetErrorHandler(EUSART_DefaultErrorHandler);

    eusartRxLastError.status = 0;

    // initializing the driver state
    eusartTxHead = 0;
    eusartTxTail = 0;
    eusartTxBufferRemaining = sizeof(eusartTxBuffer);

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;
    eusartRxDone = false;
    
    
    // enable receive interrupt

if(RC1STAbits.FERR){
        EUSART_FramingErrorHandler();
        temp_reg = RC1REG;
    }

    if(RC1STAbits.OERR){
        EUSART_OverrunErrorHandler();
        temp_reg = RC1REG;
    }


    PIE1bits.RCIE = 1;
}

bool EUSART_is_tx_ready(void)
{
    return (eusartTxBufferRemaining ? true : false);
}

bool EUSART_is_rx_ready(void)
{
    return (eusartRxCount ? true : false);
}

bool EUSART_is_tx_done(void)
{
    return TX1STAbits.TRMT;
}

eusart_status_t EUSART_get_last_status(void){
    return eusartRxLastError;
}

uint8_t EUSART_Read(void)
{
    uint8_t readValue  = 0;
    
    while(0 == eusartRxCount)
    {
    }

    eusartRxLastError = eusartRxStatusBuffer[eusartRxTail];

    readValue = eusartRxBuffer[eusartRxTail++];
    if(sizeof(eusartRxBuffer) <= eusartRxTail)
    {
        eusartRxTail = 0;
    }
    PIE1bits.RCIE = 0;
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

void EUSART_Write(uint8_t txData)
{
    while(0 == eusartTxBufferRemaining)
    {
    }

    if(0 == PIE1bits.TXIE)
    {
        TX1REG = txData;
    }
    else
    {
        PIE1bits.TXIE = 0;
        eusartTxBuffer[eusartTxHead++] = txData;
        if(sizeof(eusartTxBuffer) <= eusartTxHead)
        {
            eusartTxHead = 0;
        }
        eusartTxBufferRemaining--;
    }
    PIE1bits.TXIE = 1;
}


void EUSART_Transmit_ISR(void)
{

    // add your EUSART interrupt custom code
    if(sizeof(eusartTxBuffer) > eusartTxBufferRemaining)
    {
        TX1REG = eusartTxBuffer[eusartTxTail++];
        if(sizeof(eusartTxBuffer) <= eusartTxTail)
        {
            eusartTxTail = 0;
        }
        eusartTxBufferRemaining++;
    }
    else
    {
        PIE1bits.TXIE = 0;
    }
}

void EUSART_Receive_ISR(void)
{
    
    eusartRxStatusBuffer[eusartRxHead].status = 0;

    if(RC1STAbits.FERR){
        eusartRxStatusBuffer[eusartRxHead].ferr = 1;
        EUSART_FramingErrorHandler();
    }

    if(RC1STAbits.OERR){
        eusartRxStatusBuffer[eusartRxHead].oerr = 1;
        EUSART_OverrunErrorHandler();
    }
    
    if(eusartRxStatusBuffer[eusartRxHead].status){
        //EUSART_ErrorHandler();
        temp_reg = RC1REG;
    } else {
        EUSART_RxDataHandler();
    }
    
    // or set custom function using EUSART_SetRxInterruptHandler()
}

bool IsPacketValid(uint8_t value, uint8_t eusartRxCount)
{
    switch(eusartRxCount)
    {
        case 0:
            if (value == 0xFF)
                return true;
            else
                return false;
            break;
        case 1:
            if (value == 0x00)
                return true;
            else
                return false;
            break;
        case 2:
            if (value == 0xFF)
                return true;
            else
                return false;
            break;
        case 3:
            if (value == 0xA5)
            {
                eusartCalcChecksum += 0xA5;
                return true;
            }
            else
                return false;
            break;
        case 5:
            if ((value != GetValveRs485Address()) && (value != BROADCAST_ADDRESS))
            {
                return false;
            }
        case 4:
        case 6:
        case 7:
            eusartCalcChecksum += value;
            return true;
        case 8:
            if(value > MAX_DATA_LENGTH)
            {
                return false;
            }
            eusartRxLength = (int8_t)value; // This is to prevent a warning. Since MAX_DATA_LENGTH should be less than 0x7F this is ok
            eusartCalcChecksum += value;
            return true;
        default:
            eusartRxLength--;
            if (eusartRxLength >= 0)
            {
                eusartCalcChecksum += value;
            }
            else
            {
                eusartRxChecksum += (uint16_t)((uint16_t)value << (uint16_t)((1 - eusartRxChecksumLoc)* 8));
                if ((1 - eusartRxChecksumLoc) == 0)
                {
                    if (eusartRxChecksum != eusartCalcChecksum)
                    {
                        eusartRxHead = 0;
                        eusartRxChecksum = 0;
                        eusartCalcChecksum = 0;
                        eusartRxChecksumLoc = 0;
                        eusartRxDone = false;
                        return false;
                    }
                    else
                    {
                        //eusartRxHead = 0;
                        eusartRxChecksum = 0;
                        eusartCalcChecksum = 0;
                        eusartRxChecksumLoc = 0;
                        eusartRxDone = true;
                    }
                }
                else
                {
                    eusartRxChecksumLoc++;
                }
            }
            return true;
    }
   return false; 
}

void EUSART_RxDataHandler(void){
    // use this default receive interrupt handler code
    uint8_t temp_reg = RC1REG;
    if (IsPacketValid(temp_reg, eusartRxHead) && (eusartRxCount < MAX_PACKET))
    {
        eusartRxBuffer[eusartRxHead++] = temp_reg;
        if (eusartRxDone == true) // This is a bit odd, but we added to the q and need to move back. If not things get out of wack and next packet will end up getting corrupted.
        {
            eusartRxHead = 0;
        }
        if(sizeof(eusartRxBuffer) <= eusartRxHead)
        {
            eusartRxHead = 0;
        }
        eusartRxCount++;
    }
    else
    {
        eusartRxHead = 0;
        eusartRxCount = 0;
        eusartRxChecksum = 0;
        eusartCalcChecksum = 0;
        eusartRxChecksumLoc = 0;
        eusartRxDone = false;
    }
}

uint8_t * GetBuffer(void)
{
    return eusartRxBuffer;
}

void EUSART_DefaultFramingErrorHandler(void){
    RC1STAbits.SPEN = 0;
    RC1STAbits.SPEN = 1;
}

void EUSART_DefaultOverrunErrorHandler(void){
    // EUSART error - restart

    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;

}

void EUSART_DefaultErrorHandler(void){
    EUSART_RxDataHandler();
}

void EUSART_SetFramingErrorHandler(void (* interruptHandler)(void)){
    EUSART_FramingErrorHandler = interruptHandler;
}

void EUSART_SetOverrunErrorHandler(void (* interruptHandler)(void)){
    EUSART_OverrunErrorHandler = interruptHandler;
}

void EUSART_SetErrorHandler(void (* interruptHandler)(void)){
    EUSART_ErrorHandler = interruptHandler;
}

void EUSART_SetTxInterruptHandler(void (* interruptHandler)(void)){
    EUSART_TxDefaultInterruptHandler = interruptHandler;
}

void EUSART_SetRxInterruptHandler(void (* interruptHandler)(void)){
    EUSART_RxDefaultInterruptHandler = interruptHandler;
}
/**
  End of File
*/
