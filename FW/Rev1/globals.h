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

#ifndef GLOBALS_HEADER_H
#define	GLOBALS_HEADER_H

#include <xc.h> // include processor files - each processor file is guarded. 

// Lazy way, don't worry on the malloc; just set fixed sizes
#define MAX_DATA_LENGTH             0x50
#define MAX_PACKET                  (4 + 1 + 1 + 1 +1 +1 + MAX_DATA_LENGTH + 2)
#define EUSART_TX_BUFFER_SIZE       MAX_PACKET
#define EUSART_RX_BUFFER_SIZE       MAX_PACKET
#define BROADCAST_ADDRESS           0xF
#define VALVE_EEPROM_SERIAL_LEN     0x6

#define VALVE_MODE_NORMAL           0x4
#define VALVE_MODE_SETTINGS         0x5
#define VALVE_MODE_MAINTAINENCE     0x6
#define VALVE_MODE_REMOTE           0x20

#define MAX_DIRECTION_CHANGE        10

#define DEBUG_MODE                  0


#endif	/* GLOBALS_HEADER_H */

