/*
 * File:   debug.h
 *
 * Debug output on the UART (EUSART2 TX, 19200 bps, pin RG1).
 *
 * Off by default: DBG_PRINT() / DBG_PRINTF() compile to nothing, so the
 * strings, print() and printf() take no program memory.
 *
 * To enable, uncomment DEBUG below or add DEBUG to the compiler macros.
 * With DEBUG the transmitter is switched on in main() and the keypad leaves
 * RG1 to the UART (its bottom row 0 . = + is not scanned).
 */

#ifndef DEBUG_H
#define DEBUG_H

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#include "uart.h"
#define DBG_PRINT(s)        print(s)
#define DBG_PRINTF(...)     printf(__VA_ARGS__)
#else
#define DBG_PRINT(s)        ((void)0)
#define DBG_PRINTF(...)     ((void)0)
#endif

#endif  /* DEBUG_H */
