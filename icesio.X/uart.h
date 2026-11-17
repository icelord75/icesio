/* 
 * File:   uart.h
 * Author: ice
 *
 * Created on April 22, 2019, 10:14 AM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Debug builds (DEBUG): switch the EUSART2 transmitter on.
   Normal builds: switch EUSART2 off completely and free RG1 for the keypad. */
void Uart_Init(void);

/* Only in DEBUG builds - use DBG_PRINT() / DBG_PRINTF() from debug.h */
void print (char *string);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

