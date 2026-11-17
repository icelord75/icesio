/*
 * File:   main.h
 * Author: ice
 *
 * Created on February 5, 2019, 3:06 PM
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#define TICK_MS     100     // main loop period, ms

// Side buttons (all schematics V1/V2: MODE -> RH0/COM4, PRGM -> RH1/COM5)
#define CASIO_MODE  PORTHbits.RH0
#define CASIO_PRGM  PORTHbits.RH1

// Buzzer: SIG -> RG0/SEG42 -> T1 -> L1/BUZZ (driven by PWM3 in beep.c)
#define CASIO_SIG   LATGbits.LATG0

// TODO: the pins below don't match the schematics (K2 is RG1 = UART TX2, ...) - not used yet
#define CASIO_K2    RG0
#define CASIO_K3    RG1
#define CASIO_K4    RG2
#define CASIO_K5    RG3
#define CASIO_K6    RF7
#define CASIO_K7    RF5
#define CASIO_K8    RF3
#define CASIO_K9    RF2


#endif  /* MAIN_H */
