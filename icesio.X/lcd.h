#ifndef ICESIO_LCD_H
#define ICESIO_LCD_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>


// Bit flags for each of the eight segments.
enum lcdmap {A=1, B=2, C=4, D=8, E=0x10, F=0x20, G=0x40, DP=0x80};          // bit mask
enum lcdmapShift {As=0,Bs=1,Cs=2,Ds=3,Es=4,Fs=5,Gs=6,DPs=7};                // bit shift

enum LCDsigns {AM=1,PM=2,K=4,SIG=8,ALM=16,DIV=32,MUL=64,MINUS=128};         // bit mask
enum LCDsignsShift {AMs=0,PMs=1,Ks=2,SIGs=3,ALMs=4,DIVs=5,MULs=6,MINUSs=7}; // bit shift

enum LCDsigns2 {PLUS=1,COLON=2};                                            // bit mask
enum LCDsigns2Shift {PLUSs=0,COLONs=1};                                     // bit shift

/* Main chars:
    0-7

     AAA
    F   B
     GGG
    E   C
     DDD d

   Extra data chars:

   8   9     10

   AAA AAA  d AAA
   F   F   B  F   B
   F    GGG    GGG
   F   E   C  E   C
   DDD DDD    DDD
 */

//! Font for numbers 0..F (defined in lcd.c)
extern const int numfont[16];

// Sets a pixel.
#define DRAWPOINT(todraw) lcdm[todraw>>8]|=todraw&0xFF
// Clears a pixel.
#define CLEARPOINT(todraw) lcdm[todraw>>8]&=~(todraw&0xFF)

//! 7-segment font for ASCII 32..127 (defined in lcd.c)
extern const uint8_t ASCII[96];

extern void DrawDisplay(void);
extern void ClearDisplay(void);
extern void ClearLCD(void);
extern void DisplayPrint(char *STR);

struct LCDdef {
        uint8_t sign1,sign2,digit8,digit9,digit10;
        uint8_t digit0,digit1,digit2,digit3,digit4,digit5,digit6,digit7;
};

extern struct LCDdef Display;

#endif  /* ICESIO_LCD_H */
