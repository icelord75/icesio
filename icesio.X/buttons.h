/*
 * File:   buttons.h
 *
 * Side buttons of the CASIO 3208 module (MODE, PRGM).
 * Polled from the main loop every TICK_MS, debounced over two samples.
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

#define BTN_MODE    0x01
#define BTN_PRGM    0x02

void    Buttons_Init(void);
/* Call once per TICK_MS. Returns a bit mask of press events:
   a fresh press, plus auto-repeat events for MODE while it is held. */
uint8_t Buttons_Poll(void);
/* Buttons newly pressed at the last Buttons_Poll() (auto-repeat excluded). */
uint8_t Buttons_Pressed(void);
/* Debounced state of the buttons (bit mask of buttons held now). */
uint8_t Buttons_Held(void);

#endif  /* BUTTONS_H */
