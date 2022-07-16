#include "settings_state_controller.h"
#include "globals.h"

uint8_t valve_uid[VALVE_EEPROM_SERIAL_LEN] = {0};

volatile uint8_t currentValveAddress = 0;

void SetAddress(uint8_t newAddress)
{
    currentValveAddress = newAddress;
}

uint8_t GetAddress(void)
{
    return currentValveAddress;
}


