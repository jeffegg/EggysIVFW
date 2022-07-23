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
#include <xc.h>
#include "globals.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h" 
#include "command_system.h"
#include "ledcontroller.h"
#include "eeprom_controller.h"
#include "settings_state_controller.h"
#include "valve_manager.h"
#include "mcc_generated_files/eusart.h"

/*
                         Main application
 */

void WriteRs485Array(uint8_t * value, uint8_t length);
void SetLeds(void);
void ReadButtons(void);

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

const uint8_t valve_address_offset = 0x50;

volatile uint8_t eeprom_data[VALVE_EEPROM_SIZE] = {0};

extern volatile bool eusartRxDone;

LEDS display;
LEDS next_display;

bool modeButtonPushed = false;
bool saveButtonPushed = false;
bool yellowButtonPushed = false;
bool redButtonPushed = false;



void main(void)
{
    display.raw_leds = 0;
    next_display.raw_leds = display.raw_leds;
    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    ADC_SelectChannel(Pot1);
    
    DumpEEPROMtoMemory(eeprom_data);
    // Get the UID from the EEPROM - this should be unique per valve.
    GetUUID(valve_uid);
    
    currentValveMode = eeprom_data[mode_offset];
    nextValveMode = currentValveMode;
    
    SetAddress(eeprom_data[valve_address_offset]);
    SetLeds();
    volatile Command * newCommand;
    
    newCommand = GetCommandEntryBuffer();
       
    if (newCommand)
    {
        SetupValveAddressPackets(newCommand, BROADCAST_ADDRESS);
    }
    TransmitMessage(newCommand);
    
    
    newCommand = GetCommandEntryBuffer();
       
    if (newCommand)
    {
        newCommand->protocal = 0x1;
        newCommand->source = GetAddress();
        newCommand->destination = 0x10;
        newCommand->command = (uint8_t)VALVE_STATE;
        newCommand->data[0] = currentValveMode;
        newCommand->data[1] = currentValveLocation;
        newCommand->data_length = 2;
    }
    TransmitMessage(newCommand);
            
    ADC_StartConversion();
    
    while (1)
    {
        CLRWDT();  
        
        if (ADC_IsConversionDone())
        {
            valveADCValue = ADC_GetConversionResult();
            ADC_StartConversion();
        }
        
        if (receiveReady == true)
        {
            ReceiveCommandExecutor();
        }
        
        if (eusartRxDone == true)
        {
            uint8_t * buffer = GetBuffer();
            CopyToUARTRXBuff(buffer, eusartRxCount);
            eusartRxDone = false;
            
        }
        
        if (updateEEPROM == 1)
        {
            eeprom_data[valve_address_offset] = GetAddress();
            uint8_t valve_block[6] = {0};
            valve_block[0] = eeprom_data[valve_address_offset];
            valve_block[1] = eeprom_data[valve_address_offset + 1];
            valve_block[2] = eeprom_data[valve_address_offset + 2];
            valve_block[3] = eeprom_data[valve_address_offset + 3];
            valve_block[4] = eeprom_data[valve_address_offset + 4];
            valve_block[5] = eeprom_data[valve_address_offset + 5];
            WriteEEPROMBuffer(valve_address_offset, valve_block, 6);
            updateEEPROM = 0;
        }
        else if (updateEEPROM == 2)
        {
            eeprom_data[mode_offset] = currentValveMode;
            //WriteEEPROMBuffer(valve_address_offset, valve_block, 6);
            updateEEPROM = 0;
        }
        
        ReadButtons();
        
        if (modeButtonPushed)
        {
            nextValveMode++;
            if (nextValveMode >= 0x7)
            {
                nextValveMode = 0x4;
            }
            modeButtonPushed = false;
        }
        
        if (nextValveMode != currentValveMode)
        {
            SetLeds();
            volatile Command * newCommand;
            newCommand = GetCommandEntryBuffer();

            if (newCommand)
            {
                newCommand->protocal = 0x1;
                newCommand->source = GetAddress();
                newCommand->destination = 0x10;
                newCommand->command = (uint8_t)VALVE_STATE;
                newCommand->data[0] = nextValveMode;
                newCommand->data[1] = currentValveLocation;
                newCommand->data_length = 2;
            }
            TransmitMessage(newCommand);
            currentValveMode = nextValveMode;
            updateEEPROM = 2;
        }
        if (next_display.raw_leds != display.raw_leds)
        {
            display.raw_leds = next_display.raw_leds;
            ControlLights(&display);
        }        
    }
}



void SetLeds(void)
{
    eeprom_data[mode_offset] = nextValveMode;
    if (nextValveMode == 0x4)
        next_display.LEDbits.AUTO_LED = 1;
    else
        next_display.LEDbits.AUTO_LED = 0;

    if (nextValveMode == 0x6)
        next_display.LEDbits.SERVICE_LED = 1;
    else
        next_display.LEDbits.SERVICE_LED = 0;

    if (nextValveMode == 0x5)
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

void ReadButtons(void)
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