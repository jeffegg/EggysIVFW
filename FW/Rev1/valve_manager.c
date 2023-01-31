#include <xc.h> 
#include <stdbool.h>
#include "valve_manager.h"
#include "globals.h"
#include "mcc_generated_files/mcc.h"
#include "command_system.h"
#include "ledcontroller.h"

volatile uint16_t valveADCValue = 0xFFFF;
volatile uint8_t nextValveLocation = 5;
volatile uint8_t currentValveLocation = 5;
extern volatile uint8_t nextValveMode = 0;
extern volatile uint8_t currentValveMode = 0;
extern volatile uint8_t resetValve = 0;

extern volatile uint16_t *ADC_Endstop_24_value = 0; //0x160 on my valve
extern volatile uint16_t *ADC_Endstop_0_value = 0;  //0x6BE on my valve

// This will give the ADC value for each entry
uint16_t positionToADCTable[0x31] = {0};

void SetNextValveLocation(uint8_t newValue)
{
    if ((newValue >= 0) && (newValue <= 0x30))
        nextValveLocation = newValue;
}

uint8_t GetCurrentValveLocation(void)
{
    return currentValveLocation;
}

void SetNextValveMode(uint8_t newMode)
{
    if ((newMode >= VALVE_MODE_NORMAL) && (newMode <= VALVE_MODE_MAINTAINENCE))
        nextValveMode = newMode;
}

uint8_t GetCurrentValveMode(void)
{
    return currentValveMode;
}

void SetupValve(uint8_t storedValveLocation, uint8_t storedValveMode)
{
    CreateADCTable();
    
    if ((storedValveLocation >= 0) && (storedValveLocation <= 0x30))
        nextValveLocation = storedValveLocation;
    else
        nextValveLocation = 0x18; // Open fully up if there is a problem
    MoveValveToNewPosition();
    if ((storedValveMode >= VALVE_MODE_NORMAL) && (storedValveMode <= VALVE_MODE_MAINTAINENCE))
        nextValveMode = storedValveMode;
    else
        nextValveMode = VALVE_MODE_NORMAL;
}

bool IsRemoteEnabled(void)
{
    return (currentValveMode & VALVE_MODE_REMOTE) == VALVE_MODE_REMOTE;
}

void MoveValveToNewPosition(void)
{
    int16_t neededADCValue = positionToADCTable[nextValveLocation];
    int16_t tempADCValue = 0;
    uint8_t delayLoop1 = 0x10;
    uint8_t delayLoop2 = 0x20;
    volatile Command * newCommand;
    bool valve_ran = false;
    uint8_t direction_change = 0; // This is how many times we changed direction, will be at least 1; using this to prevent bouncing from close by values
    
    do
    {
        ADC_StartConversion();
        CLRWDT();
        while (!ADC_IsConversionDone())
        {
            
        }
        valveADCValue = ADC_GetConversionResult();
        ADC_StartConversion();
        tempADCValue = valveADCValue;
        while (!ADC_IsConversionDone())
        {
            
        }
        valveADCValue = ADC_GetConversionResult();
        tempADCValue += valveADCValue;
        
        if (debugLevel > 0x20)
        {
            
            newCommand = GetCommandEntryBuffer();

            if (newCommand)
            {
                newCommand->protocal = 0x1;
                newCommand->source = GetAddress();
                newCommand->destination = 0x10;
                newCommand->command = (uint8_t)VALVE_DEBUG_DEGREES;// Returns Byte(next position), Byte(0 - moving, 1 not moving), WORD(raw ADC read twice), WORD(neededValue);
                newCommand->data[0] = nextValveLocation;
                newCommand->data[1] = MotorB_LAT | MotorA_LAT; // Motor Location
                newCommand->data[2] = (tempADCValue >> (uint16_t)8) & 0xFF;
                newCommand->data[3] = tempADCValue & 0xFF;
                newCommand->data[4] = (neededADCValue >> (uint16_t)8) & 0xFF;
                newCommand->data[5] = neededADCValue & 0xFF;
                newCommand->data_length = 6;
            }
            TransmitMessage(newCommand);
        }
        // If we are within +/- 2of the ADCValue, we can stop. Each stop is about 0x1C off so this isn't much
        // There will be some skid, this seems to help...
        if ((tempADCValue >= (neededADCValue - 2)) && (tempADCValue <= (neededADCValue + 2)))
        {
            MotorA_SetLow();
            MotorB_SetLow();
            currentValveLocation = nextValveLocation;
            
            if (valve_ran)
            {
                newCommand = GetCommandEntryBuffer();

                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetAddress();
                    newCommand->destination = 0x10;
                    newCommand->command = (uint8_t)VALVE_DEGREES;// Returns Byte(next position), Byte(0 - moving, 1 not moving), WORD(raw ADC read twice), WORD(neededValue);
                    newCommand->data[0] = nextValveLocation;
                    newCommand->data[1] = MotorB_LAT | MotorA_LAT; // Motor Location
                    newCommand->data[2] = (tempADCValue >> (uint16_t)8) & 0xFF;
                    newCommand->data[3] = tempADCValue & 0xFF;
                    newCommand->data_length = 4;
                }
                TransmitMessage(newCommand);
                SetLeds();
            }
            
            break;
        }
        
        if (direction_change >= MAX_DIRECTION_CHANGE)
        {
            MotorA_SetLow();
            MotorB_SetLow();
            currentValveLocation = nextValveLocation;
            
            if (valve_ran)
            {
                newCommand = GetCommandEntryBuffer();

                if (newCommand)
                {
                    newCommand->protocal = 0x1;
                    newCommand->source = GetAddress();
                    newCommand->destination = 0x10;
                    newCommand->command = (uint8_t)VALVE_DEGREES;// Returns Byte(next position), Byte(0 - moving, 1 not moving), WORD(raw ADC read twice), WORD(neededValue);
                    newCommand->data[0] = nextValveLocation;
                    newCommand->data[1] = MotorB_LAT | MotorA_LAT; // Motor Location
                    newCommand->data[2] = (tempADCValue >> (uint16_t)8) & 0xFF;
                    newCommand->data[3] = tempADCValue & 0xFF;
                    newCommand->data_length = 4;
                }
                TransmitMessage(newCommand);
                SetLeds();
            }
            
            break;
        }
        
        valve_ran = true;
        
        SetLeds();
                
        if(neededADCValue < tempADCValue)
        {   
            if ((MotorB_LAT != 1) || (MotorA_LAT == 1))
            {
                MotorA_SetLow();
                do {
                    do {
                        delayLoop2 -= 1;
                    } while (delayLoop2 != 0);
                    delayLoop1 -= 1;
                } while (delayLoop1 != 0);
                MotorB_SetHigh();
                direction_change++;
            }
        }
        else
        {
            if ((MotorA_LAT != 1) || (MotorB_LAT == 1))
            {
                MotorB_SetLow();
                do {
                    do {
                        delayLoop2 -= 1;
                    } while (delayLoop2 != 0);
                    delayLoop1 -= 1;
                } while (delayLoop1 != 0);
                MotorA_SetHigh();
                direction_change++;
            }
        }   
    }   
    while(1);
    
    
}

void CreateADCTable(void)
{
    positionToADCTable[0] = *ADC_Endstop_0_value; 
    positionToADCTable[31] = *ADC_Endstop_24_value;
    float adder = (float)(*ADC_Endstop_0_value - *ADC_Endstop_24_value) / (float)0x30; // 0x62 here is 0x31 << Q; since this may be in Q format
    float temp_adder = positionToADCTable[31];

    for (int i = 0x30; i >= 0; i--)
    {
        positionToADCTable[i] = (int16_t)temp_adder;
        
        temp_adder = (float)adder + (float)temp_adder;
    }
}

/*
 * So this is the table created in the original ROM 
 * 0160 is 2x ADC at position 24
 * 06BE is 2x ADC at position 0
 * I didn't feel like figuring their code out, so I used floats
                         	BE	06	A1	06	85	06	68	06	4C
06	2F	06	12	06	F6	05	D9	05	BC	05	A0	05	83	05	67
05	4A	05	2D	05	11	05	F4	04	D7	04	BB	04	9E	04	82
04	65	04	48	04	2C	04	0F	04	F2	03	D6	03	B9	03	9D
03	80	03	63	03	47	03	2A	03	0D	03	F1	02	D4	02	B8
02	9B	02	7E	02	62	02	45	02	28	02	0C	02	EF	01	D3
01	B6	01	99	01	7D	01	60	01	

 * This was my calculation using, close enough I think
                            	BE	06	A1	06	84	06	68	06
4B	06	2E	06	12	06	F5	05	D9	05	BC	05	9F	05	83	05
66	05	49	05	2D	05	10	05	F4	04	D7	04	BA	04	9E	04
81	04	64	04	48	04	2B	04	0F	04	F2	03	D5	03	B9	03
9C	03	7F	03	63	03	46	03	2A	03	0D	03	F0	02	D4	02
B7	02	9A	02	7E	02	61	02	45	02	28	02	0B	02	EF	01
D2	01	B5	01	99	01	7C	01	60	01	

 * 
 */


