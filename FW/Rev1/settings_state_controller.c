#include "settings_state_controller.h"

volatile uint8_t currentValveAddress = 0;
void SetAddress(uint8_t newAddress)
{
    currentValveAddress = newAddress;
}
uint8_t GetAddress(void)
{
    return currentValveAddress;
}


