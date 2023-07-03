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
#include "version.h"

volatile uint32_t current_fw_version = (uint32_t)GIT_HASH;
volatile uint32_t current_fw_date = (uint32_t)GIT_HEX_DATE;
extern uint8_t *fw_version = (uint8_t *)&current_fw_version;
extern uint8_t *fw_date = (uint8_t *)&current_fw_date;
