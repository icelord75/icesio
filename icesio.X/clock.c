/*
 * File:   clock.c
 *
 * Timekeeping screen and time setting with the side buttons.
 *
 *   Time mode:     PRGM - enter setting mode
 *   Setting mode:  PRGM - next field:
 *                         hour -> minute -> second -> year -> month -> day -> exit
 *                  MODE - change the blinking field (+1, hold for auto-repeat);
 *                         on seconds - reset to 00 and start the clock
 *                         right away (press it on the time signal)
 *   Keypad (KEYPAD_TEST): the pressed key is shown as "KEY    x" for 1 s.
 *   Setting mode is also left (keeping the changes) after SET_TIMEOUT_S
 *   seconds without a press.
 *
 * Screens (main digits 0..7, extra digits 9..10):
 *
 *   digit:  0 1 2 3 4 5 6 7     9 10
 *   time:   H H : M M   S S     D  D    (D D - day of month)
 *   date:   D D - M M - Y Y              (only while setting the date)
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "lcd.h"
#include "buttons.h"
#include "beep.h"
#include "keypad.h"
#include "clock.h"

#define CLOCK_24H           1   // 1 - 24-hour display, 0 - 12-hour display with AM/PM signs
#define SET_TIMEOUT_S       60  // leave setting mode after this many seconds without a press
#define KEY_BEEP            1   // 1 - click the buzzer on every button / key press (not on auto-repeat)
#define KEYPAD_TEST         1   // 1 - show the pressed keypad key on the time screen
#define KEY_SHOW_TICKS      (1000 / TICK_MS)

#define COLON_ON_TICKS      (500 / TICK_MS)             // colon is lit for the first half of each second
#define BLINK_PERIOD_TICKS  (1000 / TICK_MS)            // blinking of the field being set
#define BLINK_ON_TICKS      (600 / TICK_MS)
#define SET_TIMEOUT_TICKS   (SET_TIMEOUT_S * (1000 / TICK_MS))

typedef enum {
        CLK_TIME = 0,       // normal time display
        CLK_SET_HOUR,
        CLK_SET_MIN,
        CLK_SET_SEC,
        CLK_SET_YEAR,
        CLK_SET_MON,
        CLK_SET_DAY
} ClockState;

static ClockState state = CLK_TIME;
static struct tm  now;          // last time read from RTCC
static struct tm  edit;         // time being set (tm_year = full year, tm_mon = 1..12)
static bool       timeApplied;  // hh:mm:00 already written to RTCC by MODE on seconds
static bool       hourEdited;   // hour changed
static bool       minEdited;    // minute changed
static bool       dateEdited;   // year, month or day changed
static int8_t     lastSec = -1; // second shown on the display last time
static uint8_t    ticks;        // colon / blink phase
static uint16_t   idleTicks;    // ticks since the last press in setting mode
static uint8_t    keyShowTicks; // keypad test: ticks left to show the pressed key

/* ---------- calendar helpers (years 2000..2099) ---------- */

static uint8_t DaysInMonth(uint8_t mon, uint16_t year)
{
        static const uint8_t dim[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (mon == 2 && (year % 4) == 0) return 29;
        return dim[mon - 1];
}

/* Day of week, 0 = Sunday (Sakamoto's method) */
static uint8_t WeekDay(uint16_t y, uint8_t m, uint8_t d)
{
        static const uint8_t t[12] = {0,3,2,5,0,3,5,1,4,6,2,4};
        if (m < 3) y--;
        return (uint8_t)((y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7);
}

static void ClampDay(struct tm *t)
{
        uint8_t dim = DaysInMonth((uint8_t)t->tm_mon, (uint16_t)t->tm_year);
        if (t->tm_mday > dim) t->tm_mday = dim;
        if (t->tm_mday < 1)   t->tm_mday = 1;
}

/* RTCC_TimeGet() can fail while the RTCC registers are updating (RTCSYNC) */
static void ReadTime(struct tm *t)
{
        while (!RTCC_TimeGet(t));
        t->tm_year = 2000 + (t->tm_year % 100);     // RTCC keeps 2 digits only
}

static void WriteTime(struct tm *t)
{
        t->tm_wday = WeekDay((uint16_t)t->tm_year, (uint8_t)t->tm_mon, (uint8_t)t->tm_mday);
        RTCC_TimeSet(t);
}

/* ---------- display ---------- */

static void Put2(uint8_t *hi, uint8_t *lo, uint8_t val, bool blankZero)
{
        if (!(blankZero && val < 10)) *hi = (uint8_t)numfont[val / 10];
        *lo = (uint8_t)numfont[val % 10];
}

/* field - the field to hide (blink), CLK_TIME for none */
static void Render(const struct tm *t, ClockState screen, ClockState field, bool colon)
{
        ClearDisplay();

        if (screen >= CLK_SET_YEAR) {
                // Date screen: DD-MM-YY
                if (field != CLK_SET_DAY)  Put2(&Display.digit0, &Display.digit1, (uint8_t)t->tm_mday, false);
                Display.digit2 = G;
                if (field != CLK_SET_MON)  Put2(&Display.digit3, &Display.digit4, (uint8_t)t->tm_mon, false);
                Display.digit5 = G;
                if (field != CLK_SET_YEAR) Put2(&Display.digit6, &Display.digit7, (uint8_t)(t->tm_year % 100), false);
        } else {
                uint8_t hour = (uint8_t)t->tm_hour;
#if !CLOCK_24H
                Display.sign1 = (hour >= 12) ? PM : AM;
                hour %= 12;
                if (hour == 0) hour = 12;
#endif
                // Hour tens blanked when zero, like the original CASIO firmware
                if (field != CLK_SET_HOUR) Put2(&Display.digit0, &Display.digit1, hour, true);
                if (field != CLK_SET_MIN)  Put2(&Display.digit3, &Display.digit4, (uint8_t)t->tm_min, false);
                if (field != CLK_SET_SEC)  Put2(&Display.digit6, &Display.digit7, (uint8_t)t->tm_sec, false);
                Display.sign2 = colon ? COLON : 0;
                if (screen == CLK_TIME)
                        Put2(&Display.digit9, &Display.digit10, (uint8_t)t->tm_mday, false);
        }

        DrawDisplay();
}

/* ---------- setting mode ---------- */

static void EnterSetting(void)
{
        ReadTime(&edit);
        timeApplied = hourEdited = minEdited = dateEdited = false;
        state = CLK_SET_HOUR;
        ticks = 0;
        idleTicks = 0;
}

static void LeaveSetting(void)
{
        struct tm cur;

        // The RTCC kept running while we were editing, so start from the live
        // time and put back only what the user has changed
        ReadTime(&cur);
        bool timeChanged = !timeApplied && (hourEdited || minEdited);
        if (timeChanged) {                          // new hour/minute, live seconds
                if (hourEdited) cur.tm_hour = edit.tm_hour;
                if (minEdited)  cur.tm_min  = edit.tm_min;
        }
        if (dateEdited) {
                cur.tm_year = edit.tm_year;
                cur.tm_mon  = edit.tm_mon;
                cur.tm_mday = edit.tm_mday;
        }
        if (timeChanged || dateEdited)
                WriteTime(&cur);

        state = CLK_TIME;
        lastSec = -1;       // force a redraw
}

static void Increment(void)
{
        switch (state) {
        case CLK_SET_HOUR:
                edit.tm_hour = (edit.tm_hour + 1) % 24;
                hourEdited = true;
                break;
        case CLK_SET_MIN:
                edit.tm_min = (edit.tm_min + 1) % 60;
                minEdited = true;
                break;
        case CLK_SET_SEC:
                // Reset seconds and start counting from this moment
                edit.tm_sec = 0;
                WriteTime(&edit);
                timeApplied = true;
                break;
        case CLK_SET_YEAR:
                edit.tm_year = 2000 + ((edit.tm_year - 2000 + 1) % 100);
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_MON:
                edit.tm_mon = (edit.tm_mon % 12) + 1;
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_DAY:
                edit.tm_mday = (edit.tm_mday % DaysInMonth((uint8_t)edit.tm_mon, (uint16_t)edit.tm_year)) + 1;
                dateEdited = true;
                break;
        default:
                break;
        }
}

/* ---------- public ---------- */

void Clock_Init(void)
{
        Buttons_Init();
        state = CLK_TIME;
        lastSec = -1;
}

bool Clock_IsSetting(void)
{
        return state != CLK_TIME;
}

void Clock_Tick(void)
{
        uint8_t pressed = Buttons_Poll();
        char    key     = Keypad_Poll();

#if KEY_BEEP
        if (Buttons_Pressed() || key) Beep_Click();
#endif

        if (state == CLK_TIME) {
                if (pressed & BTN_PRGM) {
                        keyShowTicks = 0;
                        EnterSetting();
                        Render(&edit, state, CLK_TIME, true);
                        return;
                }
#if KEYPAD_TEST
                // Keypad test: show the pressed key as "KEY    x" for a moment
                if (key) {
                        char text[9] = "KEY     ";
                        text[7] = key;
                        ClearDisplay();
                        DisplayPrint(text);
                        DrawDisplay();
                        keyShowTicks = KEY_SHOW_TICKS;
                        return;
                }
                if (keyShowTicks) {
                        if (--keyShowTicks == 0) lastSec = -1;  // back to the time
                        return;
                }
#endif
                // Redraw on every second change, blink the colon at 1 Hz
                if (!RTCC_TimeGet(&now)) return;        // RTCC registers are updating
                if (now.tm_sec != lastSec) {
                        lastSec = (int8_t)now.tm_sec;
                        ticks = 0;
                        Render(&now, CLK_TIME, CLK_TIME, true);
                } else if (++ticks == COLON_ON_TICKS) {
                        Render(&now, CLK_TIME, CLK_TIME, false);
                }
                return;
        }

        /* Setting mode */
        if (pressed) {
                idleTicks = 0;
                ticks = 0;                      // show the field right after a press
        } else if (++idleTicks >= SET_TIMEOUT_TICKS) {
                LeaveSetting();
                return;
        }

        if (pressed & BTN_PRGM) {
                if (state == CLK_SET_DAY) {
                        LeaveSetting();
                        return;
                }
                state++;
        } else if (pressed & BTN_MODE) {
                Increment();
        }

        // Redraw only when something changes: a press or a blink phase switch
        if (pressed || ticks == 0 || ticks == BLINK_ON_TICKS) {
                bool hide = (ticks >= BLINK_ON_TICKS) && !(Buttons_Held() & BTN_MODE);
                Render(&edit, state, hide ? state : CLK_TIME, true);
        }
        if (++ticks >= BLINK_PERIOD_TICKS) ticks = 0;
}
