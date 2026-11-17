/**
  RTCC Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    rtcc.h

  @Summary
    This is the generated header file for the RTCC driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs driver for RTCC.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F19197
        Driver Version    :  2.10
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

#ifndef RTCC_H
#define RTCC_H


/**
 Section: Included Files
*/


#define __builtin_write_RTCWREN() RTCCONbits.RTCWREN = 1
#define __builtin_clear_RTCWREN() RTCCONbits.RTCWREN = 0

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
 Section: Interface Routines
*/

/**
  @Summary
    Initializes and enables RTCC peripheral

  @Description
    This function sets a time in the RTCC and enables 
    RTCC for operation. It will also configure the Alarm settings.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Example
    <code>
    struct tm currentTime;

    RTCC_Initialize();

    while(!RTCC_TimeGet(&currentTime))
    {
        // Do something
    }
    </code>
*/
void RTCC_Initialize(void);

/**
  @Summary
    Returns the current time from the RTCC peripheral

  @Description
    This function returns the current time from the RTCC peripheral. This
    function uses the C library type struct tm parameter.

  @Preconditions
    None

  @Param
    currentTime - This the parameter which gets filled in by the function. The
    values are set by reading the hardware peripheral

  @Returns
    Whether the data is available or not, true if the data is available.
    false if the data is not available.

  @Example
    <code>
    struct tm currentTime;

    while(!RTCC_TimeGet(&currentTime))
    {
        // Do something
    }
    </code>
*/
bool RTCC_TimeGet(struct tm *currentTime);


/**
  @Summary
    Sets the initial time for the RTCC peripheral

  @Description
    This function sets the initial time for the RTCC peripheral. This
    function uses the C library type struct tm parameter.

  @Preconditions
    None

  @Param
    initialTime - This parameter sets the values.

  @Returns
    None

  @Example
    <code>
    struct tm initialTime;

    RTCC_TimeSet(&initialTime);
    </code>
*/
void RTCC_TimeSet(struct tm *initialTime);

/**
  @Summary
    Enables RTCC

  @Description
    This function enables RTCC module if not enabled during initialization

  @Preconditions
    RTCC_Initialize function should be called before calling this API

  @Param
    None

  @Returns
    None

  @Example
    <code>
    RTCC_Initialize();
    RTCC_Enable();
    </code>
*/
void RTCC_Enable(void);

/**
  @Summary
    Returns the alarm time from the RTCC peripheral

  @Description
    This function returns the alarm time from the RTCC peripheral. This
    function uses the C library type struct tm parameter.

  @Preconditions
    None

  @Param
    currentTime - This the parameter which gets filled in by the function. The
    values are set by reading the hardware peripheral

  @Returns
    Whether the data is available or not, true if the data is available.
    false if the data is not available.

  @Example
    <code>
    struct tm currentTime;

    while(!RTCC_AlarmTimeGet(&currentTime))
    {
        // Do something
    }
    </code>
*/
bool RTCC_AlarmTimeGet(struct tm *currentTime);


/**
  @Summary
    Sets the alarm time for the RTCC peripheral

  @Description
    This function sets the alarm time for the RTCC peripheral. This
    function uses the C library type struct tm parameter.

  @Preconditions
    None

  @Param
    alarmTime - This parameter sets the values.

  @Returns
    None

  @Example
    <code>
    struct tm alarmTime;

    RTCC_AlarmTimeSet(&alarmTime);
    </code>
*/
void RTCC_AlarmTimeSet(struct tm *alarmTime);



/**
  @Summary
    This function indicates whether to reset the RTCC value or not on  system restart.

  @Description
    This function indicates whether to reset the RTCC value or not on  system restart.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Example
    <code>
    RTCC_TimeReset();
    </code>
*/
void RTCC_TimeReset(void);

/**
  @Summary
    This function converts a hex value to bcd format
  @Description
    This function converts a hex value to bcd format


  @Preconditions
    None

  @Param
    uint8_t hexvalue

  @Returns
    None

  @Example
    <code>
	bcdTime_t bcdvalue;
	struct tm hexvalue;
	
	bcdvalue = ConvertHexToBCD(hexvalue)
    </code>
*/
uint8_t ConvertHexToBCD(uint8_t hexvalue);

/**
  @Summary
    This function converts a bcd format value to a hex value
  @Description
    This function converts a bcd format value to a hex value


  @Preconditions
    None

  @Param
    uint8_t bcdvalue

  @Returns
    None

  @Example
    <code>
	bcdTime_t bcdvalue;
	struct tm hexvalue;
	
	hexvalue = ConvertHexToBCD(bcdvalue)
    </code>
*/
uint8_t ConvertBCDToHex(uint8_t bcdvalue);

/**
  @Summary
    This function converts struct tm time to unix time
  @Description
    This function receives a tm input and returns the corresponding unix time represented in seconds since 00:00:00 on Jan1. 1970


  @Preconditions
    None

  @Param
    struct tm * tmTime

  @Returns
    None

  @Example
    <code>
	time_t unixTime;
	struct tm tmTime;
	
	unixTime = ConvertDateTimeToUnixTime(&tmTime)
    </code>
*/
time_t ConvertDateTimeToUnixTime(struct tm *tmTime);

/**
  @Summary
    This function converts unix time to struct tm type
  @Description
    This function receives a unix time input (32 bits time in seconds) and returns the corresponding tm time


  @Preconditions
    None

  @Param
    time_t * unixTime

  @Returns
    None

  @Example
    <code>
	time_t unixTime;
	struct tm tmTime;
	
	*tmTime = ConvertUnixTimeToDateTime(&unixTime)
    </code>
*/
struct tm *ConvertUnixTimeToDateTime(time_t *unixTime);

/**
  @Summary
    This function computes an suitable input for tm structure.
  @Description
    This function gets the registers values and returns a value suitable for tm structure.
    This is used for RTCC_TimeGet and RTCC_AlarmTimeGet functions.


  @Preconditions
    None

  @Param
    uint8_t reg_high
    uint8_t reg_low

  @Returns
    uint8_t 
*/
uint8_t ComputeStructureValue(uint8_t reg_high, uint8_t reg_low);

/**
  @Summary
    This function returns the decimal value of a hex. 
  @Description
    This function gets the hex value and returns the decimal corespondend.
    Example: if hex = 0x17, then the result is 17.

  @Preconditions
    None

  @Param
    hex_value

  @Returns
    uint8_t 
*/
uint8_t GetDecimalValue(uint8_t hex_value);

/**
  @Summary
    This function returns the hex value of a decimal. 
  @Description
    This function gets the decimal value and returns the hex corespondend.
    Example: if decimal = 17, then the result is 0x17.

  @Preconditions
    None

  @Param
    int value

  @Returns
    uint8_t 
*/
uint8_t GetHexValue(int value);

/**
  @Summary
    This function concat. two uint8_t values.
  @Description
    This function concat. two uint8_t values. The returned value is an int.

  @Preconditions
    None

  @Param
    uint8_t uint1
    uint8_t uint2

  @Returns
    int 
*/
int ConcatInt(uint8_t uint1, uint8_t uint2);

/**
  @Summary
    This function returns the last digit of an integer. 
  @Description
    This function returns the last digit of an integer.

  @Preconditions
    None

  @Param
    int int_value

  @Returns
    uint8_t 
*/
uint8_t GetLastDigit(int int_value);

/** Function:
  void RTCC_ISR(void)

  Summary:
    Interrupt Service Routine for the RTCC Peripheral

  Description:
    This is the interrupt service routine for the RTCC peripheral. Add in code if 
    required in the ISR. 
*/
void RTCC_ISR(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // RTCC_H

/**
 End of File
*/
