/*
 * File:   keypad.h
 *
 * 4x4 keypad of the CASIO 3208 module (CA-53W layout):
 *
 *          K6   K7   K8   K9
 *    K5     7    8    9    /
 *    K4     4    5    6    *
 *    K3     1    2    3    -
 *    K2     0    .    =    +
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

void Keypad_Init(void);
/* Call once per TICK_MS. Returns the ASCII code of a newly pressed key
   ('0'..'9', '.', '=', '+', '-', '*', '/'), or 0. */
char Keypad_Poll(void);
/* Key held now (debounced), or 0 */
char Keypad_Held(void);

#endif  /* KEYPAD_H */
