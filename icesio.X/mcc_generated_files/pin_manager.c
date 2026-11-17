/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC16F19197
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
        MPLAB             :  MPLAB X 4.15

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.
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

#include <xc.h>
#include "pin_manager.h"
#include "stdbool.h"




void (*IOCCF0_InterruptHandler)(void);
void (*IOCCF1_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATE = 0x00;
    LATD = 0x00;
    LATA = 0x00;
    LATF = 0x00;
    LATB = 0x00;
    LATG = 0x00;
    LATC = 0x00;
    LATH = 0x00;

    /**
    TRISx registers
    */
    TRISE = 0x6B;
    TRISF = 0x80;
    TRISA = 0x10;
    TRISG = 0x4D;
    TRISB = 0x00;
    TRISH = 0x0F;
    TRISC = 0x3F;
    TRISD = 0x00;

    /**
    ANSELx registers
    */
    ANSELD = 0xFF;
  // ANSELC = 0xE4;
    ANSELB = 0xFF;
    ANSELE = 0xBB;
    ANSELG = 0x8F;
    ANSELF = 0xFF;
    ANSELA = 0xCF;

    /**
    WPUx registers
    */
    WPUD = 0x00;
    WPUF = 0x00;
    WPUE = 0x00;
    WPUB = 0x00;
    WPUG = 0x00;
    WPUA = 0x00;
    WPUC = 0x00;
    WPUH = 0x00;

    /**
    ODx registers
    */
    ODCONE = 0x00;
    ODCONF = 0x00;
    ODCONA = 0x00;
    ODCONG = 0x00;
    ODCONB = 0x00;
    ODCONH = 0x00;
    ODCONC = 0x00;
    ODCOND = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0xDF;
    SLRCONB = 0xFF;
    SLRCONC = 0xFF;
    SLRCOND = 0xFF;
    SLRCONE = 0xFB;
    SLRCONF = 0xFF;
    SLRCONG = 0xDF;
    SLRCONH = 0x0F;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF0 = 0;
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF1 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN0 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN1 = 0;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP0 = 1;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP1 = 1;



    // register default IOC callback functions at runtime; use these methods to register a custom function
    IOCCF0_SetInterruptHandler(IOCCF0_DefaultInterruptHandler);
    IOCCF1_SetInterruptHandler(IOCCF1_DefaultInterruptHandler);
   
    // Enable IOCI interrupt 
    PIE0bits.IOCIE = 1; 
    
	
    SSP1CLKPPS = 0x13;   //RC3->MSSP1:SCL1;    
    RC3PPS = 0x13;   //RC3->MSSP1:SCL1;    
    SMT1WINPPS = 0x26;   //RE6->SMT1:SMT1WIN;    
    RC4PPS = 0x14;   //RC4->MSSP1:SDA1;    
    SMT1SIGPPS = 0x27;   //RE7->SMT1:SMT1SIG;    
    RG1PPS = 0x0F;   //RG1->EUSART2:TX2;    
    T0CKIPPS = 0x04;   //RA4->TMR0:T0CKI;    
    SSP1DATPPS = 0x14;   //RC4->MSSP1:SDA1;    
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCCF0
    if(IOCCFbits.IOCCF0 == 1)
    {
        IOCCF0_ISR();  
    }	
	// interrupt on change for pin IOCCF1
    if(IOCCFbits.IOCCF1 == 1)
    {
        IOCCF1_ISR();  
    }	
}

/**
   IOCCF0 Interrupt Service Routine
*/
void IOCCF0_ISR(void) {

    // Add custom IOCCF0 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF0_InterruptHandler)
    {
        IOCCF0_InterruptHandler();
    }
    IOCCFbits.IOCCF0 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF0 at application runtime
*/
void IOCCF0_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF0_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF0
*/
void IOCCF0_DefaultInterruptHandler(void){
    // add your IOCCF0 interrupt custom code
    // or set custom function using IOCCF0_SetInterruptHandler()
}

/**
   IOCCF1 Interrupt Service Routine
*/
void IOCCF1_ISR(void) {

    // Add custom IOCCF1 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF1_InterruptHandler)
    {
        IOCCF1_InterruptHandler();
    }
    IOCCFbits.IOCCF1 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF1 at application runtime
*/
void IOCCF1_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF1_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF1
*/
void IOCCF1_DefaultInterruptHandler(void){
    // add your IOCCF1 interrupt custom code
    // or set custom function using IOCCF1_SetInterruptHandler()
}

/**
 End of File
*/