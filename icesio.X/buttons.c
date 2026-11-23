/*
 * File:   buttons.c
 *
 * Side buttons of the CASIO 3208 module.
 *   MODE -> RH0, PRGM -> RH1 (see main.h)
 * Pressing a button connects the pad to GND, so the pins use the internal
 * weak pull-ups and read active-low. Same as the goodwatch board for the
 * same module: MODE/PRGM go straight to MCU pins with pull-ups enabled and
 * a press reads as 0.
 *
 * BTN_ACTIVE_LOW 0 is kept for a module whose buttons switch to VCC. The
 * PIC16F19197 has no internal pull-downs, so in that case the pins float
 * when released and need resistors to GND or the software workaround below.
 *
 * Software workaround (BTN_SOFT_PULLDOWN, active-high only): if a pin reads high, it is
 * discharged by driving it low for a couple of instruction cycles, then
 * switched back to input and read again. A floating pin stays low,
 * a pressed button pulls it back high at once.
 * While a button is held, this briefly shorts the pin to VCC through the
 * button: ~3 Tcy (~50 us at Fosc = 250 kHz) once per TICK_MS, with
 * interrupts disabled so an ISR can't stretch it. Remove it once
 * pull-down resistors (100k-1M to GND) are fitted on RH0/RH1.
 */

#include <xc.h>
#include <stdbool.h>
#include "main.h"
#include "buttons.h"

#define BTN_ACTIVE_LOW      1   // 1 - button connects the pin to GND (pull-ups), 0 - to VCC
#define BTN_SOFT_PULLDOWN   1   // active-high only: no pull-down resistors, discharge pins before reading
#define BTN_PINS            0x03                // RH0 (MODE), RH1 (PRGM) in port H
#define BTN_REPEAT_MASK     BTN_MODE            // buttons with auto-repeat
#define REPEAT_DELAY_TICKS  (600 / TICK_MS)     // hold time before auto-repeat starts
#define REPEAT_RATE_TICKS   (200 / TICK_MS)     // auto-repeat period

static uint8_t lastRaw;     // previous raw sample
static uint8_t stable;      // debounced state
static uint8_t holdTicks;   // how long the repeat buttons are held
static uint8_t fresh;       // buttons newly pressed at the last poll (no repeats)

static bool PinActive(uint8_t level)
{
#if BTN_ACTIVE_LOW
        return level == 0;
#else
        return level != 0;
#endif
}

#if !BTN_ACTIVE_LOW && BTN_SOFT_PULLDOWN
/* Discharge the floating button pins if they read high (see header) */
static void DischargePins(void)
{
        if ((PORTH & BTN_PINS) == 0) return;    // already low: released, nothing to do

        uint8_t tris = TRISH;
        uint8_t out  = tris & (uint8_t)~BTN_PINS;
        uint8_t gie  = INTCONbits.GIE;

        INTCONbits.GIE = 0;
        TRISH = out;                            // drive low (LATH0/1 = 0)
        TRISH = tris;                           // back to input
        INTCONbits.GIE = gie;
}
#endif

static uint8_t ReadRaw(void)
{
        uint8_t raw = 0;
#if !BTN_ACTIVE_LOW && BTN_SOFT_PULLDOWN
        DischargePins();
#endif
        if (PinActive(CASIO_MODE)) raw |= BTN_MODE;
        if (PinActive(CASIO_PRGM)) raw |= BTN_PRGM;
        return raw;
}

void Buttons_Init(void)
{
        // RH0/RH1 are already inputs (TRISH in pin_manager.c), but MCC leaves
        // ANSELH at its reset value (analog): digital reads would return 0
        ANSELH &= (uint8_t)~BTN_PINS;
#if !BTN_ACTIVE_LOW && BTN_SOFT_PULLDOWN
        LATH   &= (uint8_t)~BTN_PINS;           // output level for the discharge pulse
#endif
#if BTN_ACTIVE_LOW
        WPUHbits.WPUH0 = 1;     // weak pull-ups, button pulls to GND
        WPUHbits.WPUH1 = 1;
#else
        WPUHbits.WPUH0 = 0;     // pull-ups would fight the button to VCC
        WPUHbits.WPUH1 = 0;
#endif
        // A button held at power-up must not produce a press event
        lastRaw = stable = ReadRaw();
        holdTicks = 0;
}

uint8_t Buttons_Poll(void)
{
        uint8_t raw = ReadRaw();
        uint8_t events = 0;

        fresh = 0;

        if (raw == lastRaw) {                   // same level two samples in a row
                uint8_t pressed = raw & (uint8_t)~stable;
                if (pressed & BTN_REPEAT_MASK) holdTicks = 0;
                events |= pressed;
                fresh = pressed;
                stable = raw;
        }
        lastRaw = raw;

        if (stable & BTN_REPEAT_MASK) {
                if (holdTicks < 255) holdTicks++;
                if (holdTicks >= REPEAT_DELAY_TICKS + REPEAT_RATE_TICKS) {
                        holdTicks = REPEAT_DELAY_TICKS;
                        events |= stable & BTN_REPEAT_MASK;
                }
        } else {
                holdTicks = 0;
        }

        return events;
}

uint8_t Buttons_Pressed(void)
{
        return fresh;
}

uint8_t Buttons_Held(void)
{
        return stable;
}
