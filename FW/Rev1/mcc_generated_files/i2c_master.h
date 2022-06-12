/**
  I2C Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c_master.h

  @Summary
    This is the generated header file for the I2C driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for I2C.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1718
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above or later
        MPLAB             :  MPLAB X 6.00
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

#ifndef I2C_MASTER_H
#define I2C_MASTER_H

/**
  Section: Included Files
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    I2C_NOERR, // The message was sent.
    I2C_BUSY,  // Message was not sent, bus was busy.
    I2C_FAIL   // Message was not sent, bus failure
               // If you are interested in the failure reason,
               // Sit on the event call-backs.
} i2c_error_t;

typedef enum
{
    I2C_STOP=1,
    I2C_RESTART_READ,
    I2C_RESTART_WRITE,
    I2C_CONTINUE,
    I2C_RESET_LINK
} i2c_operations_t;

typedef uint8_t i2c_address_t;
typedef i2c_operations_t (*i2c_callback_t)(void *funPtr);

// common callback responses
i2c_operations_t I2C_CallbackReturnStop(void *funPtr);
i2c_operations_t I2C_CallbackReturnReset(void *funPtr);
i2c_operations_t I2C_CallbackRestartWrite(void *funPtr);
i2c_operations_t I2C_CallbackRestartRead(void *funPtr);

/**
 * \brief Initialize I2C interface
 *
 * \return Nothing
 */
void I2C_Initialize(void);

/**
 * \brief Open the I2C for communication
 *
 * \param[in] address The slave address to use in the transfer
 *
 * \return Initialization status.
 * \retval I2C_NOERR The I2C open was successful
 * \retval I2C_BUSY  The I2C open failed because the interface is busy
 * \retval I2C_FAIL  The I2C open failed with an error
 */
i2c_error_t I2C_Open(i2c_address_t address);

/**
 * \brief Close the I2C interface
 *
 * \return Status of close operation.
 * \retval I2C_NOERR The I2C open was successful
 * \retval I2C_BUSY  The I2C open failed because the interface is busy
 * \retval I2C_FAIL  The I2C open failed with an error
 */
i2c_error_t I2C_Close(void);

/**
 * \brief Start an operation on an opened I2C interface
 *
 * \param[in] read Set to true for read, false for write
 *
 * \return Status of operation
 * \retval I2C_NOERR The I2C open was successful
 * \retval I2C_BUSY  The I2C open failed because the interface is busy
 * \retval I2C_FAIL  The I2C open failed with an error
 */
i2c_error_t I2C_MasterOperation(bool read);

/**
 * \brief Identical to I2C_MasterOperation(false);
 */
i2c_error_t I2C_MasterWrite(void); // to be depreciated

/**
 * \brief Identical to I2C_MasterOperation(true);
 */
i2c_error_t I2C_MasterRead(void); // to be depreciated

/**
 * \brief Set timeout to be used for I2C operations. Uses the Timeout driver.
 *
 * \param[in] to Timeout in ticks
 *
 * \return Nothing
 */
void I2C_SetTimeout(uint8_t timeOut);

/**
 * \brief Sets up the data buffer to use, and number of bytes to transfer
 *
 * \param[in] buffer Pointer to data buffer to use for read or write data
 * \param[in] bufferSize Number of bytes to read or write from slave
 *
 * \return Nothing
 */
void I2C_SetBuffer(void *buffer, size_t bufferSize);

// Event Callback functions.

/**
 * \brief Set callback to be called when all specifed data has been transferred.
 *
 * \param[in] cb Pointer to callback function
 * \param[in] ptr  Pointer to the callback function's parameters
 *
 * \return Nothing
 */
void I2C_SetDataCompleteCallback(i2c_callback_t cb, void *ptr);

/**
 * \brief Set callback to be called when there has been a bus collision and arbitration was lost.
 *
 * \param[in] cb Pointer to callback function
 * \param[in] ptr  Pointer to the callback function's parameters
 *
 * \return Nothing
 */
void I2C_SetWriteCollisionCallback(i2c_callback_t cb, void *ptr);

/**
 * \brief Set callback to be called when the transmitted address was Nack'ed.
 *
 * \param[in] cb Pointer to callback function
 * \param[in] ptr  Pointer to the callback function's parameters
 *
 * \return Nothing
 */
void I2C_SetAddressNackCallback(i2c_callback_t cb, void *ptr);

/**
 * \brief Set callback to be called when the transmitted data was Nack'ed.
 *
 * \param[in] cb Pointer to callback function
 * \param[in] ptr  Pointer to the callback function's parameters
 *
 * \return Nothing
 */
void I2C_SetDataNackCallback(i2c_callback_t cb, void *ptr);

/**
 * \brief Set callback to be called when there was a bus timeout.
 *
 * \param[in] cb Pointer to callback function
 * \param[in] ptr  Pointer to the callback function's parameters
 *
 * \return Nothing
 */
void I2C_SetTimeoutCallback(i2c_callback_t cb, void *ptr);

#endif //I2C_MASTER_H