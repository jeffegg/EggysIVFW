/*
    EggysIVFW - Custom Firmware for Pentair's Intellivalve (TM)
    Copyright (C) 2021-2023  Jeff "Eggy" Eglinger

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
   
 */

#include <xc.h> 
#include <stdbool.h>
#include "globals.h"
#include "valve_manager.h"
#include "eeprom_controller.h"
#include "mcc_generated_files/mcc.h"
#include "command_system.h"
#include "ledcontroller.h"

void LoadValveSettingsFromEEPROM(struct ValveSettings *newValveSettings);
void LoadValveInfoFromEEPROM(struct ValveInfo *newValveInfo);
void LoadMaintencePositionFromEEPROM(uint8_t *position);
uint8_t MoveValveToNewPosition(void);
void CopyValveInfoBToA(struct ValveInfo *valveInfoA, struct ValveInfo *valveInfoB);
void ValvePosition0InterruptHandler(void);
void ValvePosition24InterruptHandler(void);
uint16_t GetCurrentADC(void);
uint8_t ADCValueToPosition(uint16_t currentDoubledADCValue);
void CreateADCTable(struct ValveSettings *currentValveSettings);

// This will give the ADC value for each entry
uint16_t positionToADCTable[0x31] = {0};
struct ValveInfo currentValveInfo;
volatile struct ValveInfo nextValveInfo;
uint8_t currentMaintenceOverridePosition = 0;
volatile uint8_t nextMaintenceOverridePosition = 0;
volatile uint8_t nextValveLocation = 0;
uint16_t valveADCValue = 0xFFFF;
uint16_t adderADC = 0;

bool lastInterruptValue = false;

const uint16_t allowedADCValueOverage = 3;

void SetupValve(void)
{
    struct ValveSettings newValveSettings;
    struct ValveInfo newValveInfo;
    LoadValveSettingsFromEEPROM(&newValveSettings);
    LoadValveInfoFromEEPROM(&newValveInfo);
    uint8_t newMaintencePosition = 0;
    LoadMaintencePositionFromEEPROM(&newMaintencePosition);
    
    valveADCValue = GetCurrentADC();
    
    CreateADCTable(&newValveSettings);
    CopyValveInfoBToA(&nextValveInfo, &newValveInfo);
    CopyValveInfoBToA(&currentValveInfo, &newValveInfo);
    nextMaintenceOverridePosition = newMaintencePosition;
    currentMaintenceOverridePosition = newMaintencePosition;
    
    // Handle White Wires
    IOCAF0_SetInterruptHandler(ValvePosition0InterruptHandler);
    // Handle Red Wires
    IOCAF1_SetInterruptHandler(ValvePosition24InterruptHandler);
}

void SetNextValveMode(ValveMode newMode)
{
    nextValveInfo.valveMode = newMode;
    if (nextValveInfo.valveMode == VALVE_MODE_MAINTAINENC)
    {
        nextMaintenceOverridePosition = GetCurrentPosition();
    }
}

void IncrementValveMode(void)
{
    ValveMode nextValveMode = GetCurrentValveMode();
    nextValveMode++;
    if (nextValveMode > VALVE_MODE_MAINTAINENC)
    {
        nextValveMode = VALVE_MODE_NORMAL;
    }
    nextValveInfo.valveMode = nextValveMode;
    if (nextValveInfo.valveMode == VALVE_MODE_MAINTAINENC)
    {
        nextMaintenceOverridePosition = GetCurrentPosition();
    }
}

ValveMode GetCurrentValveMode(void)
{
    return currentValveInfo.valveMode;
}

void SetEndstop0Value(uint8_t newEndstopValue)
{
    if (currentValveInfo.valveMode != VALVE_MODE_MAINTAINENC)
    {
        nextValveInfo.endstop0Value = newEndstopValue;
    }
}

uint8_t GetEndstop0Value(void)
{
    return currentValveInfo.endstop0Value;
}

void SetEndstop24Value(uint8_t newEndstopValue)
{
    if (currentValveInfo.valveMode != VALVE_MODE_MAINTAINENC)
    {
        nextValveInfo.endstop24Value = newEndstopValue;
    }  
}

uint8_t GetEndstop24Value(void)
{
    return currentValveInfo.endstop24Value;
}

void SetSelectedEndstop0(void)
{
    if (currentValveInfo.valveMode != VALVE_MODE_MAINTAINENC)
    {
        nextValveInfo.enstop0Selected = true;
    }
}
void SetSelectedEndstop24(void)
{
    if (currentValveInfo.valveMode != VALVE_MODE_MAINTAINENC)
    {
        nextValveInfo.enstop0Selected = false;
    }
}

SelectedEndstop GetSelectedEndstop(void)
{
    if (currentValveInfo.enstop0Selected)
    {
        return ENDSTOP_0_SELECTED;
    }
    else
    {
        return ENDSTOP_24_SELECTED;
    }
}

uint8_t GetSelectedEndstopValue(void)
{
    if (currentValveInfo.enstop0Selected)
    {
        return currentValveInfo.endstop0Value;
    }
    else
    {
        return currentValveInfo.endstop24Value;
    }
}

uint8_t GetCurrentPosition(void)
{
    valveADCValue = GetCurrentADC();
    uint16_t tempADCValue = valveADCValue;
    valveADCValue = GetCurrentADC();
    tempADCValue += valveADCValue;
        
    return ADCValueToPosition(tempADCValue);
    /*if (currentValveInfo.valveMode != VALVE_MODE_MAINTAINENC)
    {
        return currentMaintenceOverridePosition;
    }
    else
    {
        return GetSelectedEndstopValue();
    }*/
}

uint8_t PeriodicValveUpdate(void)
{
    if (currentValveInfo.valveMode != nextValveInfo.valveMode)
    {
        WriteEEPROM(VALVE_EEPROM_MODE_ADDRESS, (uint8_t)nextValveInfo.valveMode);
        // Only move valve to end stop when going to Normal version
        if (nextValveInfo.valveMode == VALVE_MODE_NORMAL)
        {
            MoveValveToNewPosition();
        }
        else if (nextValveInfo.valveMode == VALVE_MODE_MAINTAINENC)
        {
            if (currentMaintenceOverridePosition != nextMaintenceOverridePosition)
            {
                WriteEEPROM(VALVE_EEPROM_MAINTENCE_POSITION, nextMaintenceOverridePosition);
                currentMaintenceOverridePosition = nextMaintenceOverridePosition;
            }
            MoveValveToNewPosition();
        }
        currentValveInfo.valveMode = nextValveInfo.valveMode;
    }
    if (currentValveInfo.valveMode == VALVE_MODE_MAINTAINENC)
    {
        if (currentMaintenceOverridePosition != nextMaintenceOverridePosition)
        {
            WriteEEPROM(VALVE_EEPROM_MAINTENCE_POSITION, nextMaintenceOverridePosition);
            currentMaintenceOverridePosition = nextMaintenceOverridePosition;
        }
        return MoveValveToNewPosition();
    }
    else
    {
        if (currentValveInfo.endstop0Value != nextValveInfo.endstop0Value)
        {    
            WriteEEPROM(VALVE_EEPROM_0_END_STOP_ADDRESS, (uint8_t)nextValveInfo.endstop0Value);
            // Only move valve to end stop when going to Normal version; settings mode just updates
            if (nextValveInfo.valveMode == VALVE_MODE_NORMAL)
            {
                MoveValveToNewPosition();
            }
            currentValveInfo.endstop0Value = nextValveInfo.endstop0Value;
        }
        
        if (currentValveInfo.endstop24Value != nextValveInfo.endstop24Value)
        {   
            WriteEEPROM(VALVE_EEPROM_24_END_STOP_ADDRESS, (uint8_t)nextValveInfo.endstop24Value);
            // Only move valve to end stop when going to Normal version; settings mode just updates
            if (nextValveInfo.valveMode == VALVE_MODE_NORMAL)
            {
                MoveValveToNewPosition();
            }
            currentValveInfo.endstop24Value = nextValveInfo.endstop24Value;
        }
        if (nextValveInfo.enstop0Selected != currentValveInfo.enstop0Selected)
        {
            WriteEEPROM(VALVE_EEPROM_SELECTED_END_STOP_ADDRESS, (uint8_t)nextValveInfo.enstop0Selected);
            currentValveInfo.enstop0Selected = nextValveInfo.enstop0Selected;
        }
        
        return MoveValveToNewPosition();
    }
}

void LoadValveSettingsFromEEPROM(struct ValveSettings *newValveSettings)
{
    uint16_t tempADC = 0;
    ReadEEPROM((uint8_t*)&newValveSettings->endstop0ValueADC, VALVE_EEPROM_PROD_ADC_ENDSTOP_0_ADDRESS, 2);
    ReadEEPROM((uint8_t*)&newValveSettings->endstop24ValueADC, VALVE_EEPROM_PROD_ADC_ENDSTOP_24_ADDRESS, 2);
    //ReadEEPROM((uint8_t*)&newValveSettings->endstop0ValueADC, VALVE_EEPROM_PROD_BACKUP_ADC_ENDSTOP_0_ADDRESS, 2);
    //ReadEEPROM((uint8_t*)&newValveSettings->endstop24ValueADC, VALVE_EEPROM_PROD_BACKUP_ADC_ENDSTOP_24_ADDRESS, 2);
    //extern volatile uint16_t *ADC_Endstop_24_value = 0; //0x160 on my valve
    //extern volatile uint16_t *ADC_Endstop_0_value = 0;  //0x6BE on my valve

}

void LoadValveInfoFromEEPROM(struct ValveInfo *newValveInfo)
{
    uint8_t tempValue = 0;
    ReadEEPROM(&tempValue, VALVE_EEPROM_0_END_STOP_ADDRESS, 1);
    if (tempValue == 0xFF)
    {
        ReadEEPROM(&tempValue, VALVE_EEPROM_PROD_0_ENDSTOP_ADDRESS, 1);
        WriteEEPROM(VALVE_EEPROM_0_END_STOP_ADDRESS, tempValue);
    }
    newValveInfo->endstop0Value = tempValue;
    
    ReadEEPROM(&tempValue, VALVE_EEPROM_24_END_STOP_ADDRESS, 1);
    if (tempValue == 0xFF)
    {
        ReadEEPROM(&tempValue, VALVE_EEPROM_PROD_24_ENDSTOP_ADDRESS, 1);
        WriteEEPROM(VALVE_EEPROM_24_END_STOP_ADDRESS, tempValue);
    }
    newValveInfo->endstop24Value = tempValue;
    
    ReadEEPROM(&tempValue, VALVE_EEPROM_SELECTED_END_STOP_ADDRESS, 1);
    if (tempValue == 0xFF)
    {
        ReadEEPROM(&tempValue, VALVE_EEPROM_PROD_SELECTED_ENDSTOP_ADDRESS, 1);
        WriteEEPROM(VALVE_EEPROM_SELECTED_END_STOP_ADDRESS, tempValue);
    }
    newValveInfo->enstop0Selected = tempValue;
    ReadEEPROM(&tempValue, VALVE_EEPROM_MODE_ADDRESS, 1);
    if (tempValue == 0xFF)
    {
        ReadEEPROM(&tempValue, VALVE_EEPROM_PROD_MODE_ADDRESS, 1);
        WriteEEPROM(VALVE_EEPROM_MODE_ADDRESS, tempValue);
    }
    newValveInfo->valveMode = (ValveMode)tempValue;
}

void LoadMaintencePositionFromEEPROM(uint8_t *position)
{
    uint8_t tempValue = 0;
    ReadEEPROM(&tempValue, VALVE_EEPROM_MAINTENCE_POSITION, 1);
    if (tempValue == 0xFF)
    {
        tempValue = currentValveInfo.endstop0Value;
    }
    *position = tempValue;
}

uint16_t GetCurrentADC(void)
{
    ADC_StartConversion();
    while (!ADC_IsConversionDone())
    {
        CLRWDT();
    }
    return ADC_GetConversionResult();
}

uint8_t MoveValveToNewPosition(void)
{
    uint16_t neededADCValue = 0; 
    uint8_t neededPosition = 0;
    if (nextValveInfo.valveMode == VALVE_MODE_MAINTAINENC)
    {
        neededPosition = nextMaintenceOverridePosition;
    }
    else if (nextValveInfo.enstop0Selected)
    {
        neededPosition = nextValveInfo.endstop0Value;
    }
    else
    {
        neededPosition = nextValveInfo.endstop24Value;
    }
    neededADCValue = positionToADCTable[neededPosition];
    
    uint16_t currentADCValue = 0;
    
    if (GetCurrentPosition() == neededPosition)
    {
        valveADCValue = GetCurrentADC();
        currentADCValue = valveADCValue;
        valveADCValue = ADC_GetConversionResult();
        currentADCValue += valveADCValue;
        if ((currentADCValue <= (neededADCValue + 12 )) || (currentADCValue >= (neededADCValue - 12 )))
        {
            return GetCurrentPosition();
        }
    }        
    
    uint8_t delayLoop1 = 0x10;
    uint8_t delayLoop2 = 0x10;
    volatile Command * newCommand;
    bool valve_ran = false;
    uint8_t direction_change = 0; // This is how many times we changed direction, will be at least 1; using this to prevent bouncing from close by values
    
    do
    {
        valveADCValue = GetCurrentADC();
        currentADCValue = valveADCValue;
        valveADCValue = ADC_GetConversionResult();
        currentADCValue += valveADCValue;
        
        
        if (GetDebugLevel() > 0x20)
        {
        }
        
        // If we are within +/- 2of the ADCValue, we can stop. Each stop is about 0x1C off so this isn't much
        // There will be some skid, this seems to help...
        if (((currentADCValue >= (neededADCValue - allowedADCValueOverage)) && (currentADCValue <= (neededADCValue + allowedADCValueOverage))) || (direction_change >= MAX_DIRECTION_CHANGE))
        {
            MotorA_SetLow();
            MotorB_SetLow();
            //currentValveLocation = nextValveLocation;
            
            if (valve_ran)
            {
                SetLeds();
                UpdateLeds();
            }
            
            break;
        }
        valve_ran = true;  
        SetLeds();
        UpdateLeds();
        
        if(neededADCValue < currentADCValue)
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
    
    while(0);
    
    return GetCurrentPosition();
}

void CopyValveInfoBToA(struct ValveInfo *valveInfoA, struct ValveInfo *valveInfoB)
{
    valveInfoA->endstop0Value = valveInfoB->endstop0Value;
    valveInfoA->endstop24Value = valveInfoB->endstop24Value;
    valveInfoA->enstop0Selected = valveInfoB->enstop0Selected;
    valveInfoA->valveMode = valveInfoB->valveMode;
}

void ValvePosition0InterruptHandler(void)
{
    if ((!lastInterruptValue) && (!currentValveInfo.enstop0Selected))
        SetSelectedEndstop0();
    lastInterruptValue = true;
}

void ValvePosition24InterruptHandler(void)
{
    
    if ((lastInterruptValue) && (currentValveInfo.enstop0Selected))
        SetSelectedEndstop24();
    lastInterruptValue = false;
}

void CreateADCTable(struct ValveSettings *currentValveSettings)
{
    uint16_t ADC_Endstop_0_value = currentValveSettings->endstop0ValueADC; 
    uint16_t ADC_Endstop_24_value = currentValveSettings->endstop24ValueADC;
    
    positionToADCTable[0] = ADC_Endstop_0_value; 
    positionToADCTable[0x30] = ADC_Endstop_24_value;
    float adder = (float)(ADC_Endstop_0_value - ADC_Endstop_24_value) / (float)0x30; // 0x62 here is 0x31 << Q; since this may be in Q format
    adderADC = (uint16_t)adder;
    float temp_adder = positionToADCTable[0x30];

    for (int i = 0x30; i >= 0; i--)
    {
        positionToADCTable[i] = (uint16_t)temp_adder;
        
        temp_adder = (float)adder + (float)temp_adder;
    }
}

uint8_t ADCValueToPosition(uint16_t currentDoubledADCValue)
{
    uint16_t adcValueFromTable = 0;
    uint16_t prevAdcValueFromTable = positionToADCTable[0x30];
    uint8_t current_pos = 0xFF; // 0xFF value not found
    uint16_t midpoint = 0;
    
    if ((currentDoubledADCValue < (prevAdcValueFromTable - allowedADCValueOverage)) || (currentDoubledADCValue > (positionToADCTable[0x0] + allowedADCValueOverage)))
    {
        return 0xFF;
    }
    
    for (int i = 0x30; i >= 0; i--)
    {
        adcValueFromTable = positionToADCTable[i];
        if (currentDoubledADCValue == adcValueFromTable)
        {
            current_pos = (uint8_t)i;
            break;
        }
        else if (currentDoubledADCValue < adcValueFromTable)
        {
            midpoint = (uint16_t)(adcValueFromTable - prevAdcValueFromTable) >> (uint16_t)0x1;
            if ((adcValueFromTable - midpoint) >= currentDoubledADCValue)
            {
                current_pos = (uint8_t)(i + 1);
            }
            else
            {
                current_pos = (uint8_t)i;
            }
            break;
        }
        prevAdcValueFromTable = adcValueFromTable;
    }
    
    return current_pos;
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


