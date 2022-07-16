#include "eeprom_controller.h"
#include "globals.h"
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
        eeprom_data[i] = I2C_Read1ByteRegister(VALVE_EEPROM_ADDRESS_SHIFTED, i);
        CLRWDT();
    } 
}

void WriteEEPROM(uint8_t addr, uint8_t value)
{
    I2C_Write1ByteRegister(VALVE_EEPROM_ADDRESS_SHIFTED, addr, value);
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

void GetUUID(uint8_t *valve_uid)
{
    I2C_ReadDataBlock(VALVE_EEPROM_ADDRESS_SHIFTED, VALVE_EEPROM_SERIAL_ADDR, valve_uid, VALVE_EEPROM_SERIAL_LEN); 
}


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