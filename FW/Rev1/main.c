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
/*
                         Main application
 */

void WriteRs485Array(uint8_t * value, uint8_t length);
void DumpEEPROMtoMemory();
void WriteEEPROM(uint8_t addr, uint8_t value);
void WriteEEPROMBuffer(uint8_t eeprom_addr, uint8_t *buffer, uint8_t length);
uint16_t CheckSumMaker(uint8_t *buffer, uint8_t size);

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

const uint8_t VALVE_EEPROM_ADDRESS = 0xA0;
const uint8_t VALVE_EEPROM_ADDRESS_SHIFTED = VALVE_EEPROM_ADDRESS >> 1;
enum {VALVE_EEPROM_SIZE = 0x80, VALVE_EEPROM_SERIAL_LEN=0x6, VALVE_EEPROM_SERIAL_ADDR = 0xFA};

uint8_t eeprom_data[VALVE_EEPROM_SIZE] = {0};
const uint8_t first_stop_offset = 1;
const uint8_t first_stop_backup_offset = 32;
const uint8_t second_stop_offset = 3;
const uint8_t second_stop_backup_offset = 34;
const uint8_t mode_offset = 5;
const uint8_t firmware_update_mode_offset = 65;

const uint8_t did_offset = 69;
const uint8_t rid_offset = 71;

uint8_t valve_uid[6] = {VALVE_EEPROM_SERIAL_LEN};
LEDS display;

void main(void)
{
    display.raw_leds = 0;
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    DumpEEPROMtoMemory();
    // Get the UID from the EEPROM - this should be unique per valve.
    I2C_ReadDataBlock(VALVE_EEPROM_ADDRESS_SHIFTED, VALVE_EEPROM_SERIAL_ADDR, valve_uid, VALVE_EEPROM_SERIAL_LEN);

    /*int new_value = 0;
    eeprom_data[first_stop_offset] = new_value;
    eeprom_data[first_stop_backup_offset] = new_value;  
    
    new_value = 0x30;
    eeprom_data[second_stop_offset] = new_value;
    eeprom_data[second_stop_backup_offset] = new_value; 
    *(uint16_t *)(eeprom_data + 30) = CheckSumMaker(eeprom_data, 30);
    *(uint16_t *)(eeprom_data + 38) = CheckSumMaker(eeprom_data+ 32, 6);
     * WriteEEPROMBuffer(0, eeprom_data, 40);
     * */
    
    
    /*
    eeprom_data[0] = 0x00;
    eeprom_data[1] = 0x18;
    eeprom_data[2] = 0x18;
    eeprom_data[3] = 0x30;
    eeprom_data[4] = 0x01;
    eeprom_data[5] = 0x04;
    eeprom_data[6] = 0x00;
    eeprom_data[7] = 0x00;
    eeprom_data[8] = 0x00;
    eeprom_data[9] = 0x00;
    eeprom_data[10] = 0x00;
    eeprom_data[11] = 0x00;
    eeprom_data[12] = 0x00;
    eeprom_data[13] = 0x00;
    eeprom_data[14] = 0xC0;
    eeprom_data[15] = 0x02;
    eeprom_data[16] = 0x00;
    eeprom_data[17] = 0x00;
    eeprom_data[18] = 0x60;
    eeprom_data[19] = 0x01;
    eeprom_data[20] = 0xBE;
    eeprom_data[21] = 0x06;
    eeprom_data[22] = 0x29;
    eeprom_data[23] = 0x00;
    eeprom_data[24] = 0x00;
    eeprom_data[25] = 0x00;
    eeprom_data[26] = 0x00;
    eeprom_data[27] = 0x00;
    eeprom_data[28] = 0xDE;
    eeprom_data[29] = 0x02;
    eeprom_data[30] = 0xAA;
    eeprom_data[31] = 0xFC;
    eeprom_data[32] = 0x18;
    eeprom_data[33] = 0x18;
    eeprom_data[34] = 0x30;
    eeprom_data[35] = 0x01;
    eeprom_data[36] = 0x84;
    eeprom_data[37] = 0x00;
    eeprom_data[38] = 0x1A;
    eeprom_data[39] = 0xFF;
    eeprom_data[40] = 0x60;
    eeprom_data[41] = 0x01;
    eeprom_data[42] = 0xBE;
    eeprom_data[43] = 0x06;
    eeprom_data[44] = 0x04;
    eeprom_data[45] = 0x00;
    eeprom_data[46] = 0xD6;
    eeprom_data[47] = 0xFE;
    eeprom_data[48] = 0x01;
    eeprom_data[49] = 0x01;
    eeprom_data[50] = 0x01;
    eeprom_data[51] = 0x01;
    eeprom_data[52] = 0x01;
    eeprom_data[53] = 0x01;
    eeprom_data[54] = 0x01;
    eeprom_data[55] = 0x01;
    eeprom_data[56] = 0x01;
    eeprom_data[57] = 0x01;
    eeprom_data[58] = 0x01;
    eeprom_data[59] = 0xFF;
    eeprom_data[60] = 0xFF;
    eeprom_data[61] = 0xFF;
    eeprom_data[62] = 0xF7;
    eeprom_data[63] = 0xFC;
    eeprom_data[64] = 0x01;
    eeprom_data[65] = 0x01;
    eeprom_data[66] = 0x02;
    eeprom_data[67] = 0x01;
    eeprom_data[68] = 0x00;
    eeprom_data[69] = 0x30;
    eeprom_data[70] = 0x5B;
    eeprom_data[71] = 0x20;
    eeprom_data[72] = 0x01;
    eeprom_data[73] = 0xFF;
    eeprom_data[74] = 0xFF;
    eeprom_data[75] = 0xFF;
    eeprom_data[76] = 0xFF;
    eeprom_data[77] = 0xFF;
    eeprom_data[78] = 0x53;
    eeprom_data[79] = 0xFA;
     * WriteEEPROMBuffer(0, eeprom_data, 80);
        */
    
    
    if (eeprom_data[5] == 0x4)
        display.LEDbits.AUTO_LED = 1;
    else
        display.LEDbits.AUTO_LED = 0;

    if (eeprom_data[5] == 0x6)
        display.LEDbits.SERVICE_LED = 1;
    else
        display.LEDbits.SERVICE_LED = 0;
    ControlLights(&display);
    
    if (eeprom_data[5] == 0x5)
        BlueModeLed_SetLow();
    else
        BlueModeLed_SetHigh();

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

void DumpEEPROMtoMemory()
{
    for (int i= 0; i < VALVE_EEPROM_SIZE; i++)
    {
        eeprom_data[i] = I2C_Read1ByteRegister(0xA0>>1, i);
        CLRWDT();
    } 
}

void WriteEEPROM(uint8_t addr, uint8_t value)
{
    I2C_Write1ByteRegister(0xA0>>1, addr, value);
}

void WriteEEPROMBuffer(uint8_t eeprom_addr, uint8_t *buffer, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++)
    {
        WriteEEPROM(eeprom_addr + i, *(buffer + i));
        uint8_t delayLoop1 = 30;
        uint8_t delayLoop2 = 0x30;
        do {
            do {
                delayLoop2 -= 1;
            } while (delayLoop2 != 0);
            delayLoop1 -= 1;
        } while (delayLoop1 != 0);
    }
}

void SetLeds(uint16_t leds, uint8_t blank)
{
    Blank_SetLow();
}

void SetBlueModeLed(bool led_on)
{
  if (led_on != false)
      LATA &= 0x7f;
  else
      LATA |= 0x80;
  return;
}

uint16_t CheckSumMaker(uint8_t *buffer, uint8_t size)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < size; i++)
    {
        sum += *(buffer + i);
    }
    uint32_t temp2 = sum & 0xFFFF;
    uint32_t temp6 = (sum >> 16) & 0xFFFF;
    temp2 += temp6;
    
    sum = temp2;
    sum += (sum >> 16);
    return ~((uint16_t)sum);
}
/**
 End of File
*/