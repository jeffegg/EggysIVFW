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

#ifndef LEDCONTROLLER_H
#define	LEDCONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef union {
    uint16_t raw_leds;
    struct packed {
        unsigned SERVICE_LED    :1;
        unsigned LED12          :1;
        unsigned LED14          :1;
        unsigned LED16          :1;
        unsigned LED18          :1;
        unsigned LED20          :1;
        unsigned LED22          :1;
        unsigned LED24          :1;
        unsigned LED0           :1; 
        unsigned LED2           :1;
        unsigned LED4           :1;
        unsigned LED6           :1;
        unsigned LED8           :1;
        unsigned LED10          :1;
        unsigned AUTO_LED       :1;
        unsigned OFF_LED        :1;
    } LEDbits;
} LEDS;
void SetupLeds(void);
void UpdateLeds(void);
void SetLeds(void);
void IdentifyValve(void);


#endif	/* LEDCONTROLLER_H */

