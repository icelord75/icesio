/**
  @Generated PIC10 / PIC12 / PIC16 / PIC18 MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.c

  @Summary:
    This is the mcc.c file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F19197
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45 or later
        MPLAB             :  MPLAB X 4.15
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

#include "mcc.h"


void SYSTEM_Initialize(void)
{
    PMD_Initialize();
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    I2C1_Initialize();
    TMR4_Initialize();
    SMT1_Initialize();
    TMR2_Initialize();
    RTCC_Initialize();
    TMR0_Initialize();
    EUSART2_Initialize();
    LCD_Initialize();
}

void OSCILLATOR_Initialize(void)
{
    // NOSC HFINTOSC; NDIV 4; 
    OSCCON1 = 0x62;
    // CSWHOLD may proceed; SOSCPWR Low power; 
    OSCCON3 = 0x00;
    // MFOEN enabled; LFOEN disabled; ADOEN disabled; SOSCEN enabled; EXTOEN disabled; HFOEN disabled; 
    OSCEN = 0x28;
    // HFFRQ 1_MHz; 
    OSCFRQ = 0x00;
    // MFOR not ready; 
    OSCSTAT = 0x00;
    // TUN 0; 
    OSCTUNE = 0x00;
    // ACTUD enabled; ACTEN disabled; 
    ACTCON = 0x00;
}

void PMD_Initialize(void)
{
    // SYSCMD SYSCLK enabled; ACTMD ACT enabled; FVRMD FVR enabled; IOCMD IOC enabled; NVMMD NVM enabled; 
    PMD0 = 0x00;
    // TMR0MD TMR0 enabled; TMR1MD TMR1 enabled; TMR4MD TMR4 enabled; TMR2MD TMR2 enabled; 
    PMD1 = 0x00;
    // ZCDMD ZCD enabled; DACMD DAC enabled; CMP1MD CMP1 enabled; ADCMD ADC enabled; CMP2MD CMP2 enabled; RTCCMD RTCC enabled; 
    PMD2 = 0x00;
    // CCP2MD CCP2 enabled; CCP1MD CCP1 enabled; CCP4MD PWM4 enabled; CCP3MD PWM3 enabled; 
    PMD3 = 0x00;
    // CWG1MD CWG1 enabled; UART2MD EUSART2 enabled; MSSP1MD MSSP1 enabled; UART1MD EUSART enabled; 
    PMD4 = 0x00;
    // CLC3MD CLC3 enabled; CLC4MD CLC4 enabled; SMT1MD SMT1 enabled; LCDMD LCD enabled; CLC1MD CLC1 enabled; CLC2MD CLC2 enabled; 
    PMD5 = 0x00;
}


/**
 End of File
*/
