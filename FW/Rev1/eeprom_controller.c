#include "eeprom_controller.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c_master_example.h"

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

void DumpEEPROMtoMemory(uint8_t *eeprom_data)
{
    for (uint8_t i= 0; i < VALVE_EEPROM_SIZE; i++)
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
