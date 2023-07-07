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
#include "mcc_generated_files/tmr0.h"

#define MODE_BUTTON_POS 0
#define YELLOW_BUTTON_POS 1
#define RED_BUTTON_POS 2
#define SAVE_BUTTON_POS 3

// We will look for 3 samples of stables keys
uint8_t current_keys_pushed = 0;
uint8_t last_keys_pushed = 0;
uint8_t second_last_keys_pushed = 0;
uint8_t debounced_buttons = 0;

void HandleButtons(void);
void HandleButtonDebounce(void);

void ButtonManagerSetup(void)
{
    TMR0_SetDebounceInterruptHandler(HandleButtonDebounce);
}

void HandleButtonDebounce(void)
{
    if ((second_last_keys_pushed == last_keys_pushed) && (last_keys_pushed == current_keys_pushed))
    {
        debounced_buttons = current_keys_pushed;
        current_keys_pushed = 0;
        last_keys_pushed = 0;
        second_last_keys_pushed = 0;
    }
    second_last_keys_pushed = last_keys_pushed;
    last_keys_pushed = current_keys_pushed;
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
    
    current_keys_pushed = (ModeButton_GetValue() << MODE_BUTTON_POS) + 
            (YellowButton_GetValue() << YELLOW_BUTTON_POS) + 
            (RedButton_GetValue() << RED_BUTTON_POS) + 
            (SaveButton_GetValue() << SAVE_BUTTON_POS);
    HandleButtons();
}

void HandleButtons(void)
{
    bool modeButtonPushed = false;
    bool saveButtonPushed = false;
    bool yellowButtonPushed = false;
    bool redButtonPushed = false;
    bool redAndYellowButtonPushed = false;
    bool redAndSaveButtonPushed = false;

    if (debounced_buttons)
    {
        modeButtonPushed = debounced_buttons & (1 << MODE_BUTTON_POS);
        yellowButtonPushed = debounced_buttons & (1 << YELLOW_BUTTON_POS);
        redButtonPushed = debounced_buttons & (1 << RED_BUTTON_POS);
        saveButtonPushed = debounced_buttons & (1 << SAVE_BUTTON_POS);
        redAndYellowButtonPushed = redButtonPushed && yellowButtonPushed;
        redAndSaveButtonPushed = redButtonPushed && saveButtonPushed;
        debounced_buttons = 0;
        
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
}