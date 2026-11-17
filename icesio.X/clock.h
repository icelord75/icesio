/*
 * File:   clock.h
 *
 * Timekeeping screen and time setting with the PRGM / MODE buttons.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdbool.h>

void Clock_Init(void);
/* Call once per TICK_MS: polls the buttons, updates the display. */
void Clock_Tick(void);
/* true while the time setting mode is active */
bool Clock_IsSetting(void);

#endif  /* CLOCK_H */
