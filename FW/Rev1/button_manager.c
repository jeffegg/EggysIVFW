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
#include "mcc_generated_files/mcc.h"
#include "button_manager.h"
#include "valve_manager.h"
#include "settings_state_controller.h"
#include "command_system.h"

bool modeButtonPushed = false;
bool saveButtonPushed = false;
bool yellowButtonPushed = false;
bool redButtonPushed = false;
bool redAndYellowButtonPushed = false;
bool redAndSaveButtonPushed = false;

void HandleButtons(void);

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
    else if (yellowButtonPushedFirst && redButtonPushedFirst)
    {
        if(YellowButton_GetValue() == 0)
            yellowButtonPushed |= true;
        else
            yellowButtonPushed = false;
        if(RedButton_GetValue() == 0)
            redButtonPushed |= true;
        else
            redButtonPushed = false;
        if (yellowButtonPushed && redButtonPushed)
            redAndYellowButtonPushed = true;
        yellowButtonPushedFirst = false;
        redButtonPushedFirst = false; 
        yellowButtonPushed = false;
        redButtonPushed = false;
    }
    else if (saveButtonPushedFirst && redButtonPushedFirst)
    {
        if(SaveButton_GetValue() == 0)
            saveButtonPushed |= true;
        else
            saveButtonPushed = false;
        if(RedButton_GetValue() == 0)
            redButtonPushed |= true;
        else
            redButtonPushed = false;
        if (saveButtonPushed && redButtonPushed)
            redAndSaveButtonPushed = true;
        saveButtonPushedFirst = false;
        redButtonPushedFirst = false; 
        saveButtonPushed = false;
        redButtonPushed = false;
    }
    else if(saveButtonPushedFirst)
    {
        if(SaveButton_GetValue() == 0)
            saveButtonPushed |= true;
        else
            saveButtonPushed = false;
        saveButtonPushedFirst = false;
    }
    else if(yellowButtonPushedFirst)
    {
        if(YellowButton_GetValue() == 0)
            yellowButtonPushed |= true;
        else
            yellowButtonPushed = false;
        yellowButtonPushedFirst = false;
    }
    else if(redButtonPushedFirst)
    {
        if(RedButton_GetValue() == 0)
            redButtonPushed |= true;
        else
            redButtonPushed = false;
        redButtonPushedFirst = false;
    }
    
    HandleButtons();
}

void HandleButtons(void)
{
    if (modeButtonPushed)
    {   
        IncrementValveMode();
        modeButtonPushed = false;
    }  
    if (redAndYellowButtonPushed)
    {
        SelectedEndstop currentEndStop = GetSelectedEndstop();
        if (currentEndStop == ENDSTOP_0_SELECTED)
            SetSelectedEndstop24();
        else
            SetSelectedEndstop0();
        redAndYellowButtonPushed = false;
    }
    if(redAndSaveButtonPushed)
    {
        volatile Command * newCommand;          
        newCommand = GetCommandEntryBuffer();    
        if (newCommand)
        {
            SendValveHailMessage(newCommand, GetValveRs485Address(), valve_uid);
            redAndSaveButtonPushed = false;
        }
        TransmitMessage(newCommand);        
    }
}