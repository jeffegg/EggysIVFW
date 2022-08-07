/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LEDCONTROLLER_H
#define	LEDCONTROLLER_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef union {
    uint16_t raw_leds;
    struct {
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

extern volatile LEDS display;
extern volatile LEDS next_display;

void ControlLights(LEDS *param_0);
void SetLeds(void);
void SetBlueModeLed(bool led_on);


#endif	/* LEDCONTROLLER_H */

