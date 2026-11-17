/*
 * File:   beep.c
 *
 * Piezo buzzer of the CASIO 3208 module.
 *
 *   SIG (RG0) -> T1 (PDTC114, digital NPN) -> L1 (coil to VCC) -> BUZZ
 *
 * The tone is a hardware square wave: PWM3 clocked by TMR2, routed to RG0
 * through PPS. Software toggling is not possible: at Fosc = 250 kHz an
 * instruction takes 16 us, a 4 kHz half-period is only ~8 instructions.
 *
 * Between beeps RG0 is a plain output held low: T1 off, no current
 * through the coil. TMR2 and PWM3 run only while beeping.
 */

#include <xc.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"    // _XTAL_FREQ for __delay_ms
#include "beep.h"

#define BEEP_CLICK_MS   30      // key click length

/* Tone: TMR2 clock Fosc/4 = 62.5 kHz, f = 62.5 kHz / (BEEP_PR + 1)
     PR 14 -> 4167 Hz, PR 15 -> 3906 Hz, PR 30 -> 2016 Hz
   Pick the one where the piezo is loudest (its resonance). */
#define BEEP_PR         14
#define BEEP_DC         ((BEEP_PR + 1) * 2)     // 50% duty, 10-bit units (4 per timer count)

#define PPS_LATCH       0x00    // RxyPPS: pin driven by LATx
#define PPS_PWM3OUT     0x0B    // RxyPPS: PWM3 output (PIC16F19197 EDC)

void Beep_Init(void)
{
        // SIG pin: digital output, low (transistor off)
        LATGbits.LATG0   = 0;
        ANSELGbits.ANSG0 = 0;
        TRISGbits.TRISG0 = 0;
        RG0PPS = PPS_LATCH;

        // TMR2: Fosc/4, free running, 1:1, stopped (MCC set it up for HFINTOSC)
        T2CON    = 0x00;
        T2CLKCON = 0x01;
        T2HLT    = 0x00;
        T2RST    = 0x00;
        T2PR     = BEEP_PR;
        T2TMR    = 0x00;

        // PWM3 on TMR2, 50% duty, disabled
        PWM3CON  = 0x00;
        CCPTMRS0bits.P3TSEL = 1;                // 01 = TMR2
        PWM3DCH  = (uint8_t)(BEEP_DC >> 2);
        PWM3DCL  = (uint8_t)((BEEP_DC & 3) << 6);
}

void Beep_Start(void)
{
        T2TMR = 0;
        T2CONbits.ON = 1;
        PWM3CONbits.EN = 1;
        RG0PPS = PPS_PWM3OUT;
}

void Beep_Stop(void)
{
        RG0PPS = PPS_LATCH;                     // back to LATG0 = 0: transistor off
        PWM3CONbits.EN = 0;
        T2CONbits.ON = 0;
}

void Beep_Click(void)
{
        Beep_Start();
        __delay_ms(BEEP_CLICK_MS);
        Beep_Stop();
}
