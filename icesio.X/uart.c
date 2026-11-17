/*
 * File:   uart.c
 *
 * EUSART2 (TX on RG1, 19200 bps) is used only for debug output.
 * MCC still initialises it in SYSTEM_Initialize(); Uart_Init() then either
 * enables the transmitter (DEBUG) or shuts the module down.
 */

#include <xc.h>
#include "debug.h"
#include "uart.h"

#ifdef DEBUG
#include "mcc_generated_files/eusart2.h"

void Uart_Init(void){
    TX2STAbits.TXEN = 1;        // MCC leaves the transmitter off
}

void print(char *string){
    uint8_t pos=0;
    while (string[pos]!=0)
        EUSART2_Write(string[pos++]);
}

/* printf() output */
void putch(char c){
    EUSART2_Write((uint8_t)c);
}

#else

void Uart_Init(void){
    TX2STAbits.TXEN = 0;
    RC2STAbits.SPEN = 0;        // pins back to I/O
    RG1PPS = 0x00;              // RG1 driven by LATG1 (keypad row K2)
    PMD4bits.UART2MD = 1;       // module clock off
}

#endif
