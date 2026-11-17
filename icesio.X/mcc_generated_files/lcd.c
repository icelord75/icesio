/**
  LCD Generated Driver File
  
  @Company
    Microchip Technology Inc.

  @File Name
    lcd.c

  @Summary
    This is the generated driver implementation file for the LCD driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides implementations for driver APIs for LCD.
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "lcd.h"

/**
  Section: LCD APIs
*/

void LCD_Initialize(void)
{
    // Disable module before configuring
    LCDCONbits.LCDEN = 0;

    // set the LCD to the options selected in the User Interface
    // LP 1:3; WFT Type-A waveform; 
    LCDPS = 0x02;
    
    // LCDCST Max contrast (Min Resistance); 
    LCDREF = 0x00;
    
    // LRLAP disabled; LCDIRI disabled; LRLAT Always B Power mode; 
    LCDRL = 0x00;
    
    // BIAS 4.13V; EN5V enabled; LPEN enabled; 
    LCDVCON1 = 0xC2;
    
    // CPWDT disabled; LCDVSRC Charge Pump only; 
    LCDVCON2 = 0x86;
    
    //SegCom Data Registers
	LCDDATA0 = 0xFF;
	LCDDATA1 = 0x00;
	LCDDATA2 = 0x00;
	LCDDATA3 = 0x00;
	LCDDATA4 = 0x00;
	LCDDATA5 = 0x00;
	LCDDATA6 = 0x00;
	LCDDATA7 = 0x00;
	LCDDATA8 = 0x00;
	LCDDATA9 = 0x00;
	LCDDATA10 = 0x00;
	LCDDATA11 = 0x00;
	LCDDATA12 = 0x00;
	LCDDATA13 = 0x00;
	LCDDATA14 = 0x00;
	LCDDATA15 = 0x00;
	LCDDATA16 = 0x00;
	LCDDATA17 = 0x00;
	LCDDATA18 = 0x00;
	LCDDATA19 = 0x00;
	LCDDATA20 = 0x00;
	LCDDATA21 = 0x00;
	LCDDATA22 = 0x00;
	LCDDATA23 = 0x00;
	LCDDATA24 = 0x00;
	LCDDATA25 = 0x00;
	LCDDATA26 = 0x00;
	LCDDATA27 = 0x00;
	LCDDATA28 = 0x00;
	LCDDATA29 = 0x00;
	LCDDATA30 = 0x00;
	LCDDATA31 = 0x00;
	LCDDATA32 = 0x00;
	LCDDATA33 = 0x00;
	LCDDATA34 = 0x00;
	LCDDATA35 = 0x00;
	LCDDATA36 = 0x00;
	LCDDATA37 = 0x00;
	LCDDATA38 = 0x00;
	LCDDATA39 = 0x00;
	LCDDATA40 = 0x00;
	LCDDATA41 = 0x00;
	LCDDATA42 = 0x00;
	LCDDATA43 = 0x00;
	LCDDATA44 = 0x00;
	LCDDATA45 = 0x00;
	LCDDATA46 = 0x00;
	LCDDATA47 = 0x00;
    
	//Enable used segments
	LCDSE0 = 0xFF;
	LCDSE1 = 0x8F;
	LCDSE2 = 0xFC;
	LCDSE3 = 0xF9;
	LCDSE4 = 0xFE;
	LCDSE5 = 0x02;
	
    // CS LFINTOSC; SLPEN disabled; WERR No LCD Write Error; LMUX 1/4 COM(3:0); LCDEN enabled; 
    LCDCON = 0xC4;
	
    // Clearing IF flag before enabling the interrupt.
    PIR8bits.LCDIF = 0;    
    
    // Enabling LCD interrupt.
    PIE8bits.LCDIE = 1;
}

void LCD_Enable (void)
{
    LCDCONbits.LCDEN = 1;
}

void LCD_Disable (void)
{
    LCDCONbits.LCDEN = 0;    
}

void LCD_EnableSleepMode (void)
{
    LCDCONbits.SLPEN = 1;
}

void LCD_DisableSleepMode (void) 
{
    LCDCONbits.SLPEN = 0;    
}

void LCD_SetContrast (unsigned int value)
{
    LCDREFbits.LCDCST = value;
}

void LCD_SetIntervalAPowerMode (unsigned int value)
{
    LCDRLbits.LRLAP = value;      
}

void LCD_SetIntervalBPowerMode (unsigned int value)
{
    LCDRLbits.LRLBP = value;      
}

void LCD_SetPowerDistribution (unsigned int value)
{
    LCDRLbits.LRLAT = value;       
}

bool LCD_IsActive (void)
{
    return LCDPSbits.LCDA;
}

void LCD_ISR(void)
{
    // Clear interrupt flag
    PIR8bits.LCDIF = 0;
}
// end of file