/**
  SMT1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    smt1.c

  @Summary
    This is the generated driver implementation file for the SMT1 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for SMT1.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F19197
        Driver Version    :  2.11
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

#ifndef SMT1_H
#define SMT1_H

/**
  Section: Included Files
*/

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: SMT1 APIs
*/

/**
  @Summary
    Initializes the SMT1 module

  @Description
    This routine initializes the SMT1.
    This routine must be called before any other SMT1 routine is called.
    This routine should only be called once during system initialization.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment

  @Example
    <code>
    SMT1_Initialize();
    </code>
*/
void SMT1_Initialize(void);

/**
  @Summary
    Allows to enable data acquisition

  @Description
    This routine is used to enable data acquisition

  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
    </code>
*/
void SMT1_DataAcquisitionEnable(void);

/**
  @Summary
    Allows to disable data acquisition

  @Description
    This routine is used to disable data acquisition

  @Preconditions
    SMT1_Initialize() function and SMT1_DataAcquisitionEnable() 
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
        .
        .
        .
    SMT1_DataAcquisitionDisable();
    </code>
*/
void SMT1_DataAcquisitionDisable(void);

/**
  @Summary
    Allows counter to be at SMT1PR and
    interrupt occurs when clocked.

  @Description
    This routine allows the counter to be at SMT1PR and
    interrupt occurs when clocked.

  @Preconditions
    SMT1_Initialize() function and SMT1_DataAcquisitionEnable() 
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_HaltCounter();
    </code>
*/
void SMT1_HaltCounter(void);

/**
  @Summary
    Allows to change period.

  @Description
    This routine is used to change the period

  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    None

  @Param
    periodVal : Period value to be loaded

  @Example
    <code>
    SMT1_Initialize();
    SMT1_SetPeriod(0x00123456);
    </code>
*/
void SMT1_SetPeriod(uint32_t periodVal);

/**
  @Summary
    To get SMT1 period value.

  @Description
    This routine is used to get SMT1 period value.

  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    uint32_t - Value of period register.

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    SMT1_GetPeriod();
    </code>
*/
uint32_t SMT1_GetPeriod(void);

/**
  @Summary
    Allows single data acquisition.

  @Description
    This routine allows single data acquisition.

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_SingleDataAcquisition();
    </code>
*/
void SMT1_SingleDataAcquisition(void);

/**
  @Summary
    Allows repeat data acquisition.

  @Description
    This routine allows repeat data acquisition.

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_RepeatDataAcquisition();
    </code>
*/
void SMT1_RepeatDataAcquisition(void);

/**
  @Summary
    Allows SMT1 to update SMT1PR registers.

  @Description
    This routine allows SMT1 to update SMT1PR registers.

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_ManualPeriodBufferUpdate();
    </code>
*/
void SMT1_ManualPeriodBufferUpdate(void);

/**
  @Summary
    Allows SMT1 to update SMT1CPW registers.

  @Description
    This routine allows SMT1 to update SMT1CPW registers.

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_ManualPulseWidthBufferUpdate();
    </code>
*/
void SMT1_ManualPulseWidthBufferUpdate(void);

/**
  @Summary
    Allows SMT1 to reset SMT1TMR registers.

  @Description
    This routine allows SMT1 to reset SMT1TMR registers.

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    None

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();
    SMT1_DataAcquisitionEnable();
 
    // User Code
 
    SMT1_ManualTimerReset();
    </code>
*/
void SMT1_ManualTimerReset(void);

/**
  @Summary
    Determines if SMT1 window is open or closed.

  @Description
    This routine is used to determine if SMT1 window is open or closed.
    This bit is only valid when TS bit equals to 1. 

  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    1 - SMT1 window is open
    0 - SMT1 window is closed

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    if(SMT1_IsWindowOpen())
    {
        // User code..
    }
    else
    {
        // User code..
    }
    </code>
*/
bool SMT1_IsWindowOpen(void);

/**
  @Summary
    Determines if SMT1 signal acquisition is in progress or not.

  @Description
    This routine is used to determine if SMT1 signal acquisition is in progress or not.
    This bit is only valid when TS bit equals to 1. 
 
  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    1 - SMT1 signal acquisition is in progress
    0 - SMT1 signal acquisition is not in progress

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    while(!SMT1_IsSignalAcquisitionInProgress())
    {
        // User code..
    }
    </code>
*/
bool SMT1_IsSignalAcquisitionInProgress(void);

/**
  @Summary
    Determines if SMT1 timer is incrementing or not.

  @Description
    This routine is used to determine if SMT1 timer is incrementing or not.
     
  @Preconditions
    SMT1_Initialize() function and SMT1 data acquisition bit
    should have been called before calling this function.

  @Returns
    1 - SMT1 timer is incrementing
    0 - SMT1 timer is not incrementing

  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    while(!SMT1_IsTimerIncrementing())
    {
        // User code..
    }
    </code>
*/
bool SMT1_IsTimerIncrementing(void);

/**
  @Summary
    To get SMT1 pulse width latch register value.

  @Description
    This routine is used to get SMT1 pulse width latch register value.
     
  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    uint32_t - Value of pulse width latch register.
    
  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    SMT1_GetCapturedPulseWidth();
    </code>
*/
uint32_t SMT1_GetCapturedPulseWidth(void);

/**
  @Summary
    To get SMT1 period latch register value.

  @Description
    This routine is used to get SMT1 period latch register value.
     
  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    uint32_t - Value of period register.
    
  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    SMT1_GetCapturedPeriod();
    </code>
*/
uint32_t SMT1_GetCapturedPeriod(void);

/**
  @Summary
    To get SMT1 Timer latch register value.

  @Description
    This routine is used to get SMT1 Timer latch register value.
     
  @Preconditions
    SMT1_Initialize() function should have been called before calling this function.

  @Returns
    uint32_t - Value of Timer register.
    
  @Param
    None

  @Example
    <code>
    SMT1_Initialize();

    SMT1_GetTimerValue();
    </code>
*/
uint32_t SMT1_GetTimerValue(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // SMT1_H
/**
 End of File
*/


