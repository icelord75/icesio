/**
  EUSART2 Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart2.h

  @Summary
    This is the generated header file for the EUSART2 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for EUSART2.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F19197
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
        MPLAB 	          :  MPLAB X 4.15
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

#ifndef EUSART2_H
#define EUSART2_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif


/**
  Section: Macro Declarations
*/

#define EUSART2_DataReady  (EUSART2_is_rx_ready())


/**
  Section: EUSART2 APIs
*/


/**
  @Summary
    Initialization routine that takes inputs from the EUSART2 GUI.

  @Description
    This routine initializes the EUSART2 driver.
    This routine must be called before any other EUSART2 routine is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    
*/
void EUSART2_Initialize(void);

/**
  @Summary
    Checks if the EUSART2 transmitter is ready to transmit data

  @Description
    This routine checks if EUSART2 transmitter is ready 
    to accept and transmit data byte

  @Preconditions
    EUSART2_Initialize() function should have been called
    before calling this function.
    EUSART2 transmitter should be enabled before calling 
    this function

  @Param
    None

  @Returns
    Status of EUSART2 transmitter
    TRUE: EUSART2 transmitter is ready
    FALSE: EUSART2 transmitter is not ready
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART2_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART2_is_tx_ready())
                {
                    EUSART2Write(rxData);
                }
            }
        }
    }
    </code>
*/
bool EUSART2_is_tx_ready(void);

/**
  @Summary
    Checks if the EUSART2 receiver ready for reading

  @Description
    This routine checks if EUSART2 receiver has received data 
    and ready to be read

  @Preconditions
    EUSART2_Initialize() function should be called
    before calling this function
    EUSART2 receiver should be enabled before calling this 
    function

  @Param
    None

  @Returns
    Status of EUSART2 receiver
    TRUE: EUSART2 receiver is ready for reading
    FALSE: EUSART2 receiver is not ready for reading
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART2_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART2_is_tx_ready())
                {
                    EUSART2_Write(rxData);
                }
            }
        }
    }
    </code>
*/
bool EUSART2_is_rx_ready(void);

/**
  @Summary
    Checks if EUSART2 data is transmitted

  @Description
    This function return the status of transmit shift register

  @Preconditions
    EUSART2_Initialize() function should be called
    before calling this function
    EUSART2 transmitter should be enabled and EUSART2_Write
    should be called before calling this function

  @Param
    None

  @Returns
    Status of EUSART2 receiver
    TRUE: Data completely shifted out if the USART shift register
    FALSE: Data is not completely shifted out of the shift register
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            if(EUSART2_is_tx_ready())
            {
				LED_0_SetHigh();
                EUSART2Write(rxData);
            }
			if(EUSART2_is_tx_done()
            {
                LED_0_SetLow();
            }
        }
    }
    </code>
*/
bool EUSART2_is_tx_done(void);

/**
  @Summary
    Read a byte of data from the EUSART2.

  @Description
    This routine reads a byte of data from the EUSART2.

  @Preconditions
    EUSART2_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if the receiver is not empty before calling this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
*/
uint8_t EUSART2_Read(void);

 /**
  @Summary
    Writes a byte of data to the EUSART2.

  @Description
    This routine writes a byte of data to the EUSART2.

  @Preconditions
    EUSART2_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if transmitter is not busy before calling this function.

  @Param
    txData  - Data byte to write to the EUSART2

  @Returns
    None
*/
void EUSART2_Write(uint8_t txData);





#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  // EUSART2_H
/**
 End of File
*/
