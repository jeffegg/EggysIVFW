/**
  TMR0 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    tmr0.c

  @Summary
    This is the generated driver implementation file for the TMR0 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for TMR0.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1718
        Driver Version    :  2.01
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

#include <xc.h>
#include "tmr0.h"

/**
  Section: Global Variables Definitions
*/

volatile uint8_t timer0ReloadVal;
volatile uint16_t CountCallBack = 0;
volatile uint16_t debounceCountCallBack = 0;
volatile uint16_t flashLightCountCallBack = 0;
    
void (*TMR0_InterruptHandler)(void);
void (*TMR0_ProvisionedInterruptHandler)(void);
void (*TMR0_DebounceInterruptHandler)(void);
void (*TMR0_LightFlashInterruptHandler)(void);
/**
  Section: TMR0 APIs
*/

void TMR0_Initialize(void)
{
    // Set TMR0 to the options selected in the User Interface
	
    // PSA assigned; PS 1:128; TMRSE Increment_hi_lo; mask the nWPUEN and INTEDG bits
    OPTION_REG = (uint8_t)((OPTION_REG & 0xC0) | (0x96 & 0x3F)); 
	
    // TMR0 194; 
    TMR0 = 0xC2;
    CountCallBack = 0;
    debounceCountCallBack = 0;
    flashLightCountCallBack = 0;
	
    // Load the TMR value to reload variable
    timer0ReloadVal= 0xC2;
        
    // Set Default Interrupt Handler
    TMR0_SetInterruptHandler(TMR0_DefaultInterruptHandler);
    TMR0_SetProvisionedInterruptHandler(TMR0_DefaultProvisionedInterruptHandler);
    TMR0_SetDebounceInterruptHandler(TMR0_DefaultDebounceInterruptHandler);
    TMR0_SetLightFlashInterruptHandler(TMR0_DefaultLightFlashInterruptHandler);

    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt
    INTCONbits.TMR0IE = 1;

}

uint8_t TMR0_ReadTimer(void)
{
    uint8_t readVal;

    readVal = TMR0;

    return readVal;
}

void TMR0_WriteTimer(uint8_t timerVal)
{
    // Write to the Timer0 register
    TMR0 = timerVal;
}

void TMR0_Reload(void)
{
    // Write to the Timer0 register
    TMR0 = timer0ReloadVal;
}

void TMR0_ISR(void)
{


    TMR0 = timer0ReloadVal;
    
    if(TMR0_InterruptHandler)
    {
        TMR0_InterruptHandler();
    }
    // callback function - called every 30242th pass
    if (++CountCallBack >= TMR0_INTERRUPT_TICKER_FACTOR)
    {
        // ticker function call
        if (TMR0_ProvisionedInterruptHandler)
        {
            TMR0_ProvisionedInterruptHandler();
        }
        // reset ticker counter
        CountCallBack = 0;
    }
    
    if (++debounceCountCallBack >= TMR0_INTERRUPT_DEBOUNCE_FACTOR)
    {
        if (TMR0_DebounceInterruptHandler)
        {
            TMR0_DebounceInterruptHandler();
        }
        // reset ticker counter
        debounceCountCallBack = 0;
    }
    
    if (++flashLightCountCallBack >= TMR0_INTERRUPT_LIGHT_FLASH_FACTOR)
    {
        if (TMR0_LightFlashInterruptHandler)
        {
            TMR0_LightFlashInterruptHandler();
        }
        // reset ticker counter
        flashLightCountCallBack = 0;
    }
    
    // Clear the TMR0 interrupt flag after the interrupt is handled
    INTCONbits.TMR0IF = 0;
}

void TMR0_SetDebounceInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_DebounceInterruptHandler = InterruptHandler;
}

void TMR0_DefaultDebounceInterruptHandler(void)
{
    // Add your custom callback code here called every 30 seconds
}

void TMR0_SetProvisionedInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_ProvisionedInterruptHandler = InterruptHandler;
}

void TMR0_DefaultProvisionedInterruptHandler(void)
{
    // Add your custom callback code here called every 30 seconds
}

void TMR0_SetInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_InterruptHandler = InterruptHandler;
}

void TMR0_DefaultInterruptHandler(void){
    // add your TMR0 interrupt custom code
    // or set custom function using TMR0_SetInterruptHandler()
}

void TMR0_SetLightFlashInterruptHandler(void (* InterruptHandler)(void)){
    TMR0_LightFlashInterruptHandler = InterruptHandler;
}

void TMR0_DefaultLightFlashInterruptHandler(void){
}

/**
  End of File
*/
