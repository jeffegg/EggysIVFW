/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1718
        Driver Version    :  2.00
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

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h"
#include "command_system.h"
#include "ledcontroller.h"
#include "eeprom_controller.h"
#include "settings_state_controller.h"
/*
                         Main application
 */

void WriteRs485Array(uint8_t * value, uint8_t length);
void SetLeds();
void ReadButtons();

enum
{
    POR         = 0x1,
    ILLEGAL     = 0x2,
    BOR         = 0x3,
    WDT_RESET   = 0x4,
    WDT_WAKE    = 0x5,
    INT_WAKE    = 0x6,
    MCLR        = 0x7,
    MCLR_SLEEP  = 0x8,
    RESET       = 0x9,
    STACK_OVER  = 0xA,
    STACK_UNDER = 0xB,
    BAD_VAL     = 0xC
} Reset_Reason;



const uint8_t first_stop_offset = 1;
const uint8_t first_stop_backup_offset = 32;
const uint8_t second_stop_offset = 3;
const uint8_t second_stop_backup_offset = 34;
const uint8_t mode_offset = 5;
const uint8_t firmware_update_mode_offset = 65;

const uint8_t did_offset = 69;
const uint8_t rid_offset = 71;

const uint8_t unused_offset = 0x50;
const uint8_t unused_offset_length = 0x80 - 0x50; // 48 bytes available to us


volatile uint8_t eeprom_data[VALVE_EEPROM_SIZE] = {0};
volatile uint8_t valve_uid[VALVE_EEPROM_SERIAL_LEN] = {0};
LEDS display;
LEDS next_display;

bool modeButtonPushed = false;
bool saveButtonPushed = false;
bool yellowButtonPushed = false;
bool redButtonPushed = false;

uint8_t nextMode = 0;

void main(void)
{
    display.raw_leds = 0;
    next_display.raw_leds = display.raw_leds;
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    DumpEEPROMtoMemory(eeprom_data);
    // Get the UID from the EEPROM - this should be unique per valve.
    GetUUID(valve_uid);
    
    nextMode = eeprom_data[mode_offset];
    SetLeds();
    Command * newCommand;
    GetCommandEntryBuffer(newCommand);
    if (newCommand)
    {
        newCommand->command = (uint8_t)VALVE_STATE;
    }
    TransmitMessage(newCommand);
    
    while (1)
    {
        CLRWDT();      
        ReadButtons();
        
        if (modeButtonPushed)
        {
            nextMode++;
            if (nextMode >= 0x7)
            {
                nextMode = 0x4;
            }
            modeButtonPushed = false;
        }
        
        if (nextMode != eeprom_data[mode_offset])
        {
            SetLeds();
        }
        if (next_display.raw_leds != display.raw_leds)
        {
            display.raw_leds = next_display.raw_leds;
            ControlLights(&display);
        }
        
        
    }
    
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

void SetLeds()
{
    eeprom_data[mode_offset] = nextMode;
    if (nextMode == 0x4)
        next_display.LEDbits.AUTO_LED = 1;
    else
        next_display.LEDbits.AUTO_LED = 0;

    if (nextMode == 0x6)
        next_display.LEDbits.SERVICE_LED = 1;
    else
        next_display.LEDbits.SERVICE_LED = 0;

    if (nextMode == 0x5)
        BlueModeLed_SetLow();
    else
        BlueModeLed_SetHigh();
}

void SetBlueModeLed(bool led_on)
{
  if (led_on != false)
      LATA &= 0x7f;
  else
      LATA |= 0x80;
  return;
}

void ReadButtons()
{
    bool modeButtonPushedFirst = false;
    bool saveButtonPushedFirst = false;
    bool yellowButtonPushedFirst = false;
    bool redButtonPushedFirst = false;

    modeButtonPushedFirst |= (ModeButton_GetValue() == 0);
    saveButtonPushedFirst |= (SaveButton_GetValue() == 0);
    yellowButtonPushedFirst |= (YellowButton_GetValue() == 0);
    redButtonPushedFirst |= (RedButton_GetValue() == 0);
    
    // TODO - needs to move to a timer
    // Delay for about 50 mS-100mS
    uint8_t delayLoop1 = 0xF0;
    uint8_t delayLoop2 = 0xF0;
    do {
        do {
            delayLoop2 -= 1;
        } while (delayLoop2 != 0);
        delayLoop1 -= 1;
    } while (delayLoop1 != 0);

    if(modeButtonPushedFirst)
    {
        if (ModeButton_GetValue() == 0)
            modeButtonPushed |= true;
        else
            modeButtonPushed = false;
        modeButtonPushedFirst = false;
    }
    if(saveButtonPushedFirst)
    {
        if(SaveButton_GetValue() == 0)
            saveButtonPushed |= true;
        else
            saveButtonPushed = false;
        saveButtonPushedFirst = false;
    }
    if(yellowButtonPushedFirst)
    {
        if(YellowButton_GetValue() == 0)
            yellowButtonPushed |= true;
        else
            yellowButtonPushed = false;
        yellowButtonPushedFirst = false;
    }
    if(redButtonPushedFirst)
    {
        if(RedButton_GetValue() == 0)
            redButtonPushed |= true;
        else
            redButtonPushed = false;
        redButtonPushedFirst = false;
    }
}

/**
 End of File
*/