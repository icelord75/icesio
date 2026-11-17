/*
 * File:   beep.h
 *
 * Piezo buzzer of the CASIO 3208 module, driven by PWM3 through T1.
 */

#ifndef BEEP_H
#define BEEP_H

void Beep_Init(void);
void Beep_Start(void);
void Beep_Stop(void);
/* Short key click, blocks for BEEP_CLICK_MS */
void Beep_Click(void);
/* Long beep for a rejected input, blocks for BEEP_ERROR_MS */
void Beep_Error(void);

#endif  /* BEEP_H */
