/*
 * File:   keypad.c
 *
 * 4x4 keypad of the CASIO 3208 module.
 *
 *   Rows    (driven):  K2 = RG1, K3 = RG2, K4 = RG3, K5 = RF7
 *   Columns (read):    K6 = RC7, K7 = RC6, K8 = RC0, K9 = RC1
 *
 * Key matrix taken from the goodwatch firmware (same 3208 module).
 *
 * Columns are inputs with weak pull-ups. Between scans all rows are driven
 * low, so any pressed key pulls its column low: a single PORTC read tells
 * whether anything is pressed (and the columns are on port C, which has
 * interrupt-on-change for a future wake-up from sleep). To find the key,
 * one row at a time is driven low while the others are left floating.
 *
 * Pin conflicts resolved in Keypad_Init():
 *   - RG1 is the EUSART2 TX pin (PPS). Normally the keypad takes it over;
 *     with DEBUG (debug.h) the UART keeps it and the bottom row (0 . = +)
 *     is not scanned.
 *   - RC0/RC1 are the SOSC crystal pins: the secondary oscillator is
 *     switched off (the RTCC runs from MFINTOSC).
 *   - RC6/RC7 were enabled as LCD segments SEG27/SEG28 (not used by the
 *     display): they are returned to digital I/O.
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "keypad.h"
#include "debug.h"

// 1 - scan the bottom row (takes RG1 from the UART), 0 - keep UART TX on RG1.
// Debug builds (DEBUG in debug.h) keep the UART.
#ifdef DEBUG
#define KEYPAD_ROW_K2   0
#else
#define KEYPAD_ROW_K2   1
#endif

/* Port bits */
#define COL_MASK        0xC3    // RC7, RC6, RC1, RC0
#define ROW_G_MASK      0x0E    // RG1, RG2, RG3
#define ROW_F_MASK      0x80    // RF7

#if KEYPAD_ROW_K2
#define ROWS            4
#else
#define ROWS            3       // K2 (RG1) left to the UART
#endif

/* Row r (0 = K5 ... 3 = K2): port G mask / port F mask */
static const uint8_t rowG[4] = { 0x00, 0x08, 0x04, 0x02 };     // K5 is on port F
static const uint8_t rowF[4] = { 0x80, 0x00, 0x00, 0x00 };

/* Column c (0 = K6 ... 3 = K9): bit in PORTC */
static const uint8_t colBit[4] = { 0x80, 0x40, 0x01, 0x02 };

static const char keymap[4][4] = {
        { '7', '8', '9', '/' },         // K5
        { '4', '5', '6', '*' },         // K4
        { '1', '2', '3', '-' },         // K3
        { '0', '.', '=', '+' },         // K2
};

static char lastRaw;        // previous raw sample
static char stable;         // debounced key

#if KEYPAD_ROW_K2
#define ROWS_G_USED     ROW_G_MASK
#else
#define ROWS_G_USED     (ROW_G_MASK & (uint8_t)~0x02)
#endif

/* Idle state: all rows driven low */
static void RowsIdle(void)
{
        TRISG &= (uint8_t)~ROWS_G_USED;
        TRISF &= (uint8_t)~ROW_F_MASK;
}

/* Raw scan: the pressed key, or 0 for none / more than one */
static char Scan(void)
{
        char key = 0;
        uint8_t found = 0;

        if ((PORTC & COL_MASK) == COL_MASK) return 0;   // nothing pressed

        for (uint8_t r = 0; r < ROWS; r++) {
                // All rows floating, then only row r driven low
                TRISG |= ROWS_G_USED;
                TRISF |= ROW_F_MASK;
                TRISG &= (uint8_t)~rowG[r];
                TRISF &= (uint8_t)~rowF[r];
                NOP();                                  // let the columns settle
                uint8_t cols = PORTC;

                for (uint8_t c = 0; c < 4; c++) {
                        if ((cols & colBit[c]) == 0) {
                                key = keymap[r][c];
                                found++;
                        }
                }
        }
        RowsIdle();

        return (found == 1) ? key : 0;          // ignore chords and ghosting
}

void Keypad_Init(void)
{
        // Columns: digital inputs with weak pull-ups.
        // LCDSEx may only be changed with the LCD module off.
        uint8_t lcdOn = LCDCONbits.LCDEN;
        LCDCONbits.LCDEN = 0;
        LCDSE3  &= (uint8_t)~0x18;              // SEG27 (RC6), SEG28 (RC7) off
        LCDCONbits.LCDEN = lcdOn;
        OSCENbits.SOSCEN = 0;                   // free RC0/RC1 from the SOSC
        ANSELC  &= (uint8_t)~COL_MASK;
        TRISC   |= COL_MASK;
        WPUC    |= COL_MASK;

        // Rows: digital outputs, low
#if KEYPAD_ROW_K2
        RG1PPS = 0x00;                          // RG1: LATG1 instead of EUSART2 TX
#endif
        ANSELG  &= (uint8_t)~ROWS_G_USED;
        ANSELF  &= (uint8_t)~ROW_F_MASK;
        LATG    &= (uint8_t)~ROWS_G_USED;
        LATF    &= (uint8_t)~ROW_F_MASK;
        RowsIdle();

        lastRaw = stable = Scan();              // a key held at power-up is not a press
}

char Keypad_Poll(void)
{
        char raw = Scan();
        char event = 0;

        if (raw == lastRaw && raw != stable) {  // same reading two ticks in a row
                if (raw) event = raw;
                stable = raw;
        }
        lastRaw = raw;

        return event;
}

char Keypad_Held(void)
{
        return stable;
}
