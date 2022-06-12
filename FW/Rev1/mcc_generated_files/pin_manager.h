/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1718
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set channel_AN0 aliases
#define channel_AN0_TRIS                 TRISAbits.TRISA0
#define channel_AN0_LAT                  LATAbits.LATA0
#define channel_AN0_PORT                 PORTAbits.RA0
#define channel_AN0_WPU                  WPUAbits.WPUA0
#define channel_AN0_OD                   ODCONAbits.ODA0
#define channel_AN0_ANS                  ANSELAbits.ANSA0
#define channel_AN0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define channel_AN0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define channel_AN0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define channel_AN0_GetValue()           PORTAbits.RA0
#define channel_AN0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define channel_AN0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define channel_AN0_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define channel_AN0_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define channel_AN0_SetPushPull()        do { ODCONAbits.ODA0 = 0; } while(0)
#define channel_AN0_SetOpenDrain()       do { ODCONAbits.ODA0 = 1; } while(0)
#define channel_AN0_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define channel_AN0_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set RedPin aliases
#define RedPin_TRIS                 TRISAbits.TRISA1
#define RedPin_LAT                  LATAbits.LATA1
#define RedPin_PORT                 PORTAbits.RA1
#define RedPin_WPU                  WPUAbits.WPUA1
#define RedPin_OD                   ODCONAbits.ODA1
#define RedPin_ANS                  ANSELAbits.ANSA1
#define RedPin_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define RedPin_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define RedPin_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define RedPin_GetValue()           PORTAbits.RA1
#define RedPin_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define RedPin_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define RedPin_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define RedPin_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define RedPin_SetPushPull()        do { ODCONAbits.ODA1 = 0; } while(0)
#define RedPin_SetOpenDrain()       do { ODCONAbits.ODA1 = 1; } while(0)
#define RedPin_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define RedPin_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set MotorA aliases
#define MotorA_TRIS                 TRISAbits.TRISA4
#define MotorA_LAT                  LATAbits.LATA4
#define MotorA_PORT                 PORTAbits.RA4
#define MotorA_WPU                  WPUAbits.WPUA4
#define MotorA_OD                   ODCONAbits.ODA4
#define MotorA_ANS                  ANSELAbits.ANSA4
#define MotorA_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define MotorA_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define MotorA_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define MotorA_GetValue()           PORTAbits.RA4
#define MotorA_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define MotorA_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define MotorA_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define MotorA_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define MotorA_SetPushPull()        do { ODCONAbits.ODA4 = 0; } while(0)
#define MotorA_SetOpenDrain()       do { ODCONAbits.ODA4 = 1; } while(0)
#define MotorA_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define MotorA_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set MotorB aliases
#define MotorB_TRIS                 TRISAbits.TRISA5
#define MotorB_LAT                  LATAbits.LATA5
#define MotorB_PORT                 PORTAbits.RA5
#define MotorB_WPU                  WPUAbits.WPUA5
#define MotorB_OD                   ODCONAbits.ODA5
#define MotorB_ANS                  ANSELAbits.ANSA5
#define MotorB_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define MotorB_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define MotorB_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define MotorB_GetValue()           PORTAbits.RA5
#define MotorB_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define MotorB_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define MotorB_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define MotorB_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define MotorB_SetPushPull()        do { ODCONAbits.ODA5 = 0; } while(0)
#define MotorB_SetOpenDrain()       do { ODCONAbits.ODA5 = 1; } while(0)
#define MotorB_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define MotorB_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

// get/set IO_RA6 aliases
#define IO_RA6_TRIS                 TRISAbits.TRISA6
#define IO_RA6_LAT                  LATAbits.LATA6
#define IO_RA6_PORT                 PORTAbits.RA6
#define IO_RA6_WPU                  WPUAbits.WPUA6
#define IO_RA6_OD                   ODCONAbits.ODA6
#define IO_RA6_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define IO_RA6_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define IO_RA6_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define IO_RA6_GetValue()           PORTAbits.RA6
#define IO_RA6_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define IO_RA6_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)
#define IO_RA6_SetPullup()          do { WPUAbits.WPUA6 = 1; } while(0)
#define IO_RA6_ResetPullup()        do { WPUAbits.WPUA6 = 0; } while(0)
#define IO_RA6_SetPushPull()        do { ODCONAbits.ODA6 = 0; } while(0)
#define IO_RA6_SetOpenDrain()       do { ODCONAbits.ODA6 = 1; } while(0)

// get/set BlueModeLed aliases
#define BlueModeLed_TRIS                 TRISAbits.TRISA7
#define BlueModeLed_LAT                  LATAbits.LATA7
#define BlueModeLed_PORT                 PORTAbits.RA7
#define BlueModeLed_WPU                  WPUAbits.WPUA7
#define BlueModeLed_OD                   ODCONAbits.ODA7
#define BlueModeLed_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define BlueModeLed_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define BlueModeLed_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define BlueModeLed_GetValue()           PORTAbits.RA7
#define BlueModeLed_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define BlueModeLed_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)
#define BlueModeLed_SetPullup()          do { WPUAbits.WPUA7 = 1; } while(0)
#define BlueModeLed_ResetPullup()        do { WPUAbits.WPUA7 = 0; } while(0)
#define BlueModeLed_SetPushPull()        do { ODCONAbits.ODA7 = 0; } while(0)
#define BlueModeLed_SetOpenDrain()       do { ODCONAbits.ODA7 = 1; } while(0)

// get/set Blank aliases
#define Blank_TRIS                 TRISBbits.TRISB0
#define Blank_LAT                  LATBbits.LATB0
#define Blank_PORT                 PORTBbits.RB0
#define Blank_WPU                  WPUBbits.WPUB0
#define Blank_OD                   ODCONBbits.ODB0
#define Blank_ANS                  ANSELBbits.ANSB0
#define Blank_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define Blank_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define Blank_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define Blank_GetValue()           PORTBbits.RB0
#define Blank_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define Blank_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define Blank_SetPullup()          do { WPUBbits.WPUB0 = 1; } while(0)
#define Blank_ResetPullup()        do { WPUBbits.WPUB0 = 0; } while(0)
#define Blank_SetPushPull()        do { ODCONBbits.ODB0 = 0; } while(0)
#define Blank_SetOpenDrain()       do { ODCONBbits.ODB0 = 1; } while(0)
#define Blank_SetAnalogMode()      do { ANSELBbits.ANSB0 = 1; } while(0)
#define Blank_SetDigitalMode()     do { ANSELBbits.ANSB0 = 0; } while(0)

// get/set Pot1 aliases
#define Pot1_TRIS                 TRISBbits.TRISB1
#define Pot1_LAT                  LATBbits.LATB1
#define Pot1_PORT                 PORTBbits.RB1
#define Pot1_WPU                  WPUBbits.WPUB1
#define Pot1_OD                   ODCONBbits.ODB1
#define Pot1_ANS                  ANSELBbits.ANSB1
#define Pot1_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define Pot1_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define Pot1_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define Pot1_GetValue()           PORTBbits.RB1
#define Pot1_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define Pot1_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define Pot1_SetPullup()          do { WPUBbits.WPUB1 = 1; } while(0)
#define Pot1_ResetPullup()        do { WPUBbits.WPUB1 = 0; } while(0)
#define Pot1_SetPushPull()        do { ODCONBbits.ODB1 = 0; } while(0)
#define Pot1_SetOpenDrain()       do { ODCONBbits.ODB1 = 1; } while(0)
#define Pot1_SetAnalogMode()      do { ANSELBbits.ANSB1 = 1; } while(0)
#define Pot1_SetDigitalMode()     do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set Pot2 aliases
#define Pot2_TRIS                 TRISBbits.TRISB2
#define Pot2_LAT                  LATBbits.LATB2
#define Pot2_PORT                 PORTBbits.RB2
#define Pot2_WPU                  WPUBbits.WPUB2
#define Pot2_OD                   ODCONBbits.ODB2
#define Pot2_ANS                  ANSELBbits.ANSB2
#define Pot2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define Pot2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define Pot2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define Pot2_GetValue()           PORTBbits.RB2
#define Pot2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define Pot2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define Pot2_SetPullup()          do { WPUBbits.WPUB2 = 1; } while(0)
#define Pot2_ResetPullup()        do { WPUBbits.WPUB2 = 0; } while(0)
#define Pot2_SetPushPull()        do { ODCONBbits.ODB2 = 0; } while(0)
#define Pot2_SetOpenDrain()       do { ODCONBbits.ODB2 = 1; } while(0)
#define Pot2_SetAnalogMode()      do { ANSELBbits.ANSB2 = 1; } while(0)
#define Pot2_SetDigitalMode()     do { ANSELBbits.ANSB2 = 0; } while(0)

// get/set Pot3 aliases
#define Pot3_TRIS                 TRISBbits.TRISB3
#define Pot3_LAT                  LATBbits.LATB3
#define Pot3_PORT                 PORTBbits.RB3
#define Pot3_WPU                  WPUBbits.WPUB3
#define Pot3_OD                   ODCONBbits.ODB3
#define Pot3_ANS                  ANSELBbits.ANSB3
#define Pot3_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define Pot3_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define Pot3_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define Pot3_GetValue()           PORTBbits.RB3
#define Pot3_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define Pot3_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define Pot3_SetPullup()          do { WPUBbits.WPUB3 = 1; } while(0)
#define Pot3_ResetPullup()        do { WPUBbits.WPUB3 = 0; } while(0)
#define Pot3_SetPushPull()        do { ODCONBbits.ODB3 = 0; } while(0)
#define Pot3_SetOpenDrain()       do { ODCONBbits.ODB3 = 1; } while(0)
#define Pot3_SetAnalogMode()      do { ANSELBbits.ANSB3 = 1; } while(0)
#define Pot3_SetDigitalMode()     do { ANSELBbits.ANSB3 = 0; } while(0)

// get/set DE aliases
#define DE_TRIS                 TRISBbits.TRISB4
#define DE_LAT                  LATBbits.LATB4
#define DE_PORT                 PORTBbits.RB4
#define DE_WPU                  WPUBbits.WPUB4
#define DE_OD                   ODCONBbits.ODB4
#define DE_ANS                  ANSELBbits.ANSB4
#define DE_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define DE_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define DE_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define DE_GetValue()           PORTBbits.RB4
#define DE_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define DE_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define DE_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define DE_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define DE_SetPushPull()        do { ODCONBbits.ODB4 = 0; } while(0)
#define DE_SetOpenDrain()       do { ODCONBbits.ODB4 = 1; } while(0)
#define DE_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define DE_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set nRE aliases
#define nRE_TRIS                 TRISBbits.TRISB5
#define nRE_LAT                  LATBbits.LATB5
#define nRE_PORT                 PORTBbits.RB5
#define nRE_WPU                  WPUBbits.WPUB5
#define nRE_OD                   ODCONBbits.ODB5
#define nRE_ANS                  ANSELBbits.ANSB5
#define nRE_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define nRE_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define nRE_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define nRE_GetValue()           PORTBbits.RB5
#define nRE_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define nRE_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define nRE_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define nRE_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define nRE_SetPushPull()        do { ODCONBbits.ODB5 = 0; } while(0)
#define nRE_SetOpenDrain()       do { ODCONBbits.ODB5 = 1; } while(0)
#define nRE_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define nRE_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

// get/set ModeButton aliases
#define ModeButton_TRIS                 TRISCbits.TRISC0
#define ModeButton_LAT                  LATCbits.LATC0
#define ModeButton_PORT                 PORTCbits.RC0
#define ModeButton_WPU                  WPUCbits.WPUC0
#define ModeButton_OD                   ODCONCbits.ODC0
#define ModeButton_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define ModeButton_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define ModeButton_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define ModeButton_GetValue()           PORTCbits.RC0
#define ModeButton_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define ModeButton_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define ModeButton_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define ModeButton_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define ModeButton_SetPushPull()        do { ODCONCbits.ODC0 = 0; } while(0)
#define ModeButton_SetOpenDrain()       do { ODCONCbits.ODC0 = 1; } while(0)

// get/set RedButton aliases
#define RedButton_TRIS                 TRISCbits.TRISC1
#define RedButton_LAT                  LATCbits.LATC1
#define RedButton_PORT                 PORTCbits.RC1
#define RedButton_WPU                  WPUCbits.WPUC1
#define RedButton_OD                   ODCONCbits.ODC1
#define RedButton_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define RedButton_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define RedButton_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define RedButton_GetValue()           PORTCbits.RC1
#define RedButton_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define RedButton_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define RedButton_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define RedButton_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define RedButton_SetPushPull()        do { ODCONCbits.ODC1 = 0; } while(0)
#define RedButton_SetOpenDrain()       do { ODCONCbits.ODC1 = 1; } while(0)

// get/set SaveButton aliases
#define SaveButton_TRIS                 TRISCbits.TRISC2
#define SaveButton_LAT                  LATCbits.LATC2
#define SaveButton_PORT                 PORTCbits.RC2
#define SaveButton_WPU                  WPUCbits.WPUC2
#define SaveButton_OD                   ODCONCbits.ODC2
#define SaveButton_ANS                  ANSELCbits.ANSC2
#define SaveButton_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define SaveButton_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define SaveButton_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define SaveButton_GetValue()           PORTCbits.RC2
#define SaveButton_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define SaveButton_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define SaveButton_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define SaveButton_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define SaveButton_SetPushPull()        do { ODCONCbits.ODC2 = 0; } while(0)
#define SaveButton_SetOpenDrain()       do { ODCONCbits.ODC2 = 1; } while(0)
#define SaveButton_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define SaveButton_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set RC3 procedures
#define RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()              PORTCbits.RC3
#define RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define RC3_SetPullup()             do { WPUCbits.WPUC3 = 1; } while(0)
#define RC3_ResetPullup()           do { WPUCbits.WPUC3 = 0; } while(0)
#define RC3_SetAnalogMode()         do { ANSELCbits.ANSC3 = 1; } while(0)
#define RC3_SetDigitalMode()        do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set LAT aliases
#define LAT_TRIS                 TRISCbits.TRISC5
#define LAT_LAT                  LATCbits.LATC5
#define LAT_PORT                 PORTCbits.RC5
#define LAT_WPU                  WPUCbits.WPUC5
#define LAT_OD                   ODCONCbits.ODC5
#define LAT_ANS                  ANSELCbits.ANSC5
#define LAT_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define LAT_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define LAT_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define LAT_GetValue()           PORTCbits.RC5
#define LAT_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define LAT_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define LAT_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define LAT_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define LAT_SetPushPull()        do { ODCONCbits.ODC5 = 0; } while(0)
#define LAT_SetOpenDrain()       do { ODCONCbits.ODC5 = 1; } while(0)
#define LAT_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define LAT_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

// get/set DI aliases
#define DI_TRIS                 TRISCbits.TRISC6
#define DI_LAT                  LATCbits.LATC6
#define DI_PORT                 PORTCbits.RC6
#define DI_WPU                  WPUCbits.WPUC6
#define DI_OD                   ODCONCbits.ODC6
#define DI_ANS                  ANSELCbits.ANSC6
#define DI_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define DI_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define DI_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define DI_GetValue()           PORTCbits.RC6
#define DI_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define DI_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define DI_SetPullup()          do { WPUCbits.WPUC6 = 1; } while(0)
#define DI_ResetPullup()        do { WPUCbits.WPUC6 = 0; } while(0)
#define DI_SetPushPull()        do { ODCONCbits.ODC6 = 0; } while(0)
#define DI_SetOpenDrain()       do { ODCONCbits.ODC6 = 1; } while(0)
#define DI_SetAnalogMode()      do { ANSELCbits.ANSC6 = 1; } while(0)
#define DI_SetDigitalMode()     do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RO aliases
#define RO_TRIS                 TRISCbits.TRISC7
#define RO_LAT                  LATCbits.LATC7
#define RO_PORT                 PORTCbits.RC7
#define RO_WPU                  WPUCbits.WPUC7
#define RO_OD                   ODCONCbits.ODC7
#define RO_ANS                  ANSELCbits.ANSC7
#define RO_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RO_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RO_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RO_GetValue()           PORTCbits.RC7
#define RO_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RO_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RO_SetPullup()          do { WPUCbits.WPUC7 = 1; } while(0)
#define RO_ResetPullup()        do { WPUCbits.WPUC7 = 0; } while(0)
#define RO_SetPushPull()        do { ODCONCbits.ODC7 = 0; } while(0)
#define RO_SetOpenDrain()       do { ODCONCbits.ODC7 = 1; } while(0)
#define RO_SetAnalogMode()      do { ANSELCbits.ANSC7 = 1; } while(0)
#define RO_SetDigitalMode()     do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF0 pin functionality
 * @Example
    IOCAF0_ISR();
 */
void IOCAF0_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF0 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF0 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF0_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF0_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF0 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF0_SetInterruptHandler() method.
    This handler is called every time the IOCAF0 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF0_SetInterruptHandler(IOCAF0_InterruptHandler);

*/
extern void (*IOCAF0_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF0 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF0_SetInterruptHandler() method.
    This handler is called every time the IOCAF0 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF0_SetInterruptHandler(IOCAF0_DefaultInterruptHandler);

*/
void IOCAF0_DefaultInterruptHandler(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCAF1 pin functionality
 * @Example
    IOCAF1_ISR();
 */
void IOCAF1_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCAF1 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCAF1 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF1_SetInterruptHandler(MyInterruptHandler);

*/
void IOCAF1_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCAF1 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCAF1_SetInterruptHandler() method.
    This handler is called every time the IOCAF1 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF1_SetInterruptHandler(IOCAF1_InterruptHandler);

*/
extern void (*IOCAF1_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCAF1 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCAF1_SetInterruptHandler() method.
    This handler is called every time the IOCAF1 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCAF1_SetInterruptHandler(IOCAF1_DefaultInterruptHandler);

*/
void IOCAF1_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/