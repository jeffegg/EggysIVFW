#include "valve_manager.h"

volatile uint16_t valveADCValue = 0xFFFF;
volatile uint8_t nextValveLocation = 0;
volatile uint8_t currentValveLocation = 23;
extern volatile uint8_t nextValveMode = 0;
extern volatile uint8_t currentValveMode = 0;