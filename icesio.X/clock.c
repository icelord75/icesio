/*
 * File:   clock.c
 *
 * Timekeeping screen and time setting with the side buttons.
 *
 *   Time mode:     PRGM - enter setting mode
 *   Setting mode:  PRGM - next field:
 *                         hour -> minute -> second -> year -> month -> day ->
 *                         rate correction (COR) -> exit
 *                  MODE - change the blinking field (+1, hold for auto-repeat);
 *                         on seconds - reset to 00 and start the clock
 *                         right away (press it on the time signal)
 *   Keypad in setting mode:
 *                  0..9 - type the field: two digits set it and move on to
 *                         the next field ("1_" after the first digit);
 *                         hours are typed as 00..23, the year as 00..99;
 *                         typing seconds starts the clock at hh:mm:ss
 *                  =    - like PRGM (a single typed digit is taken as 0x)
 *                  + -  - change the field by one (on seconds: reset to 00)
 *                  .    - finish setting
 *                  An impossible value (hour 25, 31 April...) is rejected
 *                  with a long beep.
 *   COR field:     rate correction in seconds per day, "Cor  -12":
 *                  MODE / + : +1, - : -1; hold to repeat, after 3 s in
 *                  steps of 10. Positive = the watch is slow (adds time).
 *                  Kept in data EEPROM.
 *   Keypad in time mode (KEYPAD_TEST): the key is shown as "KEY    x" for 1 s.
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

/* Rate correction: the RTCC runs from MFINTOSC (no 32 kHz crystal), which can
   be off by up to ~2 % (~1700 s/day). RTCCAL only reaches about +-6 s/day,
   so the correction is done in software: every elapsed second adds COR to an
   accumulator, and each time it reaches one day (86400) the RTCC seconds are
   moved by one. */
#define COR_MAX             1999                        // s/day, display "COR-1999"
#define COR_REPEAT_TICKS    (600 / TICK_MS)             // hold before auto-repeat
#define COR_FAST_TICKS      (3000 / TICK_MS)            // hold before steps of 10
#define EE_COR              0xF000                      // data EEPROM: magic, low, high
#define EE_COR_MAGIC        0xC5

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
        CLK_SET_DAY,
        CLK_SET_COR
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
static uint8_t    entryCount;   // digits typed into the current field (0 or 1)
static uint8_t    entryValue;   // the digit typed so far

static int16_t    corr;         // rate correction, s/day (+ = watch is slow)
static int16_t    corrSaved;    // value in EEPROM
static int32_t    corrAcc;      // accumulated error, 1/86400 s
static int8_t     corrLastSec = -1;
static uint8_t    corHold;      // COR field: ticks the +/- input is held

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

/* ---------- rate correction ---------- */

static void CorrLoad(void)
{
        corr = 0;
        if (DATAEE_ReadByte(EE_COR) == EE_COR_MAGIC) {
                int16_t v = (int16_t)(DATAEE_ReadByte(EE_COR + 1) | (DATAEE_ReadByte(EE_COR + 2) << 8));
                if (v >= -COR_MAX && v <= COR_MAX) corr = v;
        }
        corrSaved = corr;
}

static void CorrSave(void)
{
        if (corr == corrSaved) return;          // spare the EEPROM
        DATAEE_WriteByte(EE_COR + 1, (uint8_t)corr);
        DATAEE_WriteByte(EE_COR + 2, (uint8_t)((uint16_t)corr >> 8));
        DATAEE_WriteByte(EE_COR, EE_COR_MAGIC);
        corrSaved = corr;
        corrAcc = 0;
}

/* Move the RTCC seconds by +1 / -1 without stopping it. Called right after
   a seconds change, far from the next rollover (RTCSYNC = 0), and only when
   the minute is not affected. */
static void RtccNudge(uint8_t sec)
{
        __builtin_write_RTCWREN();
        SECONDS = (uint8_t)(((sec / 10) << 4) | (sec % 10));
        __builtin_clear_RTCWREN();
}

/* Call with every fresh RTCC reading while not in setting mode.
   Returns true if the seconds were moved (the screen needs a redraw). */
static bool CorrTick(const struct tm *t)
{
        int8_t sec = (int8_t)t->tm_sec;

        if (corrLastSec < 0) {                  // first reading after start / setting
                corrLastSec = sec;
                return false;
        }
        uint8_t elapsed = (uint8_t)((sec - corrLastSec + 60) % 60);
        if (elapsed == 0) return false;
        corrLastSec = sec;

        if (corr == 0) {
                corrAcc = 0;
                return false;
        }
        while (elapsed--) corrAcc += corr;      // usually 1: avoids a 32-bit multiply

        // COR is the drift per real day. A watch off by COR s/day counts
        // 86400 - COR of its own seconds in a real day, so that is the
        // threshold (using 86400 would leave an error of COR^2/86400 s/day).
        int32_t day = 86400L - corr;

        // Only nudge within the minute (seconds 0..58 up, 1..59 down);
        // otherwise wait for the next second
        if (corrAcc >= day && sec <= 58 && !RTCCONbits.RTCSYNC) {
                corrAcc -= day;
                corrLastSec = sec + 1;
                RtccNudge((uint8_t)(sec + 1));
                return true;
        }
        if (corrAcc <= -day && sec >= 1 && !RTCCONbits.RTCSYNC) {
                corrAcc += day;
                corrLastSec = sec - 1;
                RtccNudge((uint8_t)(sec - 1));
                return true;
        }
        return false;
}

/* COR field input: +1 / -1 with auto-repeat, steps of 10 after a long hold.
   Wraps around so the side buttons alone (MODE = +) can reach negative values. */
static void CorrStep(int8_t by)
{
        int16_t v = corr + by;
        if (v > COR_MAX)  v -= 2 * COR_MAX + 1;
        if (v < -COR_MAX) v += 2 * COR_MAX + 1;
        corr = v;
}

static void CorrInput(uint8_t pressed, char key)
{
        int8_t dir = 0;
        bool fresh = (Buttons_Pressed() & BTN_MODE) || key == '+' || key == '-';

        if ((Buttons_Held() & BTN_MODE) || Keypad_Held() == '+') dir = +1;
        else if (Keypad_Held() == '-')                           dir = -1;
        if (key == '+') dir = +1;
        if (key == '-') dir = -1;
        (void)pressed;

        if (fresh) {
                corHold = 0;
                CorrStep(dir);
        } else if (dir) {
                if (corHold < 255) corHold++;
                if (corHold >= COR_REPEAT_TICKS && ((corHold - COR_REPEAT_TICKS) & 1) == 0)
                        CorrStep((int8_t)(corHold >= COR_FAST_TICKS ? dir * 10 : dir));
        } else {
                corHold = 0;
        }
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

        if (screen == CLK_SET_COR) {
                // "COR" and the value right-aligned, "-" in front if negative
                Display.digit0 = ASCII['C' - 32];
                Display.digit1 = ASCII['o' - 32];      // capital O would look like 0
                Display.digit2 = ASCII['R' - 32];
                if (field != CLK_SET_COR) {
                        uint16_t v = (uint16_t)(corr < 0 ? -corr : corr);
                        uint8_t *d = &Display.digit7;
                        do {
                                *d-- = (uint8_t)numfont[v % 10];
                                v /= 10;
                        } while (v);
                        if (corr < 0) *d = G;
                }
        } else if (screen >= CLK_SET_YEAR) {
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

        // One digit typed into the field being set: show it as "d_"
        if (entryCount && screen == state) {
                uint8_t *hi, *lo;
                switch (state) {
                case CLK_SET_HOUR: case CLK_SET_DAY:  hi = &Display.digit0; lo = &Display.digit1; break;
                case CLK_SET_MIN:  case CLK_SET_MON:  hi = &Display.digit3; lo = &Display.digit4; break;
                default:                              hi = &Display.digit6; lo = &Display.digit7; break;
                }
                *hi = (uint8_t)numfont[entryValue];
                *lo = D;                        // underscore: waiting for the second digit
        }

        DrawDisplay();
}

/* ---------- setting mode ---------- */

static void EnterSetting(void)
{
        ReadTime(&edit);
        timeApplied = hourEdited = minEdited = dateEdited = false;
        state = CLK_SET_HOUR;
        entryCount = 0;
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

        CorrSave();
        corrLastSec = -1;   // restart counting from the new time
        state = CLK_TIME;
        lastSec = -1;       // force a redraw
}

/* Add 'by' to a value in 0..n-1, wrapping around */
static uint8_t Wrap(uint8_t val, int8_t by, uint8_t n)
{
        return (uint8_t)((val + n + by) % n);
}

/* Change the current field by +1 / -1 */
static void Step(int8_t dir)
{
        entryCount = 0;
        switch (state) {
        case CLK_SET_HOUR:
                edit.tm_hour = Wrap((uint8_t)edit.tm_hour, dir, 24);
                hourEdited = true;
                break;
        case CLK_SET_MIN:
                edit.tm_min = Wrap((uint8_t)edit.tm_min, dir, 60);
                minEdited = true;
                break;
        case CLK_SET_SEC:
                // Reset seconds and start counting from this moment
                edit.tm_sec = 0;
                WriteTime(&edit);
                timeApplied = true;
                break;
        case CLK_SET_YEAR:
                edit.tm_year = 2000 + Wrap((uint8_t)(edit.tm_year - 2000), dir, 100);
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_MON:
                edit.tm_mon = Wrap((uint8_t)(edit.tm_mon - 1), dir, 12) + 1;
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_DAY:
                edit.tm_mday = Wrap((uint8_t)(edit.tm_mday - 1), dir,
                                    DaysInMonth((uint8_t)edit.tm_mon, (uint16_t)edit.tm_year)) + 1;
                dateEdited = true;
                break;
        default:
                break;
        }
}

/* Set the current field to a typed value. false if the value is impossible. */
static bool SetField(uint8_t v)
{
        switch (state) {
        case CLK_SET_HOUR:
                if (v > 23) return false;
                edit.tm_hour = v;
                hourEdited = true;
                break;
        case CLK_SET_MIN:
                if (v > 59) return false;
                edit.tm_min = v;
                minEdited = true;
                break;
        case CLK_SET_SEC:
                if (v > 59) return false;
                edit.tm_sec = v;
                WriteTime(&edit);               // start the clock at hh:mm:ss now
                timeApplied = true;
                break;
        case CLK_SET_YEAR:
                if (v > 99) return false;
                edit.tm_year = 2000 + v;
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_MON:
                if (v < 1 || v > 12) return false;
                edit.tm_mon = v;
                ClampDay(&edit);
                dateEdited = true;
                break;
        case CLK_SET_DAY:
                if (v < 1 || v > DaysInMonth((uint8_t)edit.tm_mon, (uint16_t)edit.tm_year)) return false;
                edit.tm_mday = v;
                dateEdited = true;
                break;
        default:
                return false;
        }
        return true;
}

/* Go to the next field. true if setting mode was left. */
static bool NextField(void)
{
        entryCount = 0;
        if (state == CLK_SET_COR) {
                LeaveSetting();
                return true;
        }
        state++;
        return false;
}

/* PRGM / '=': take a pending single digit, then move on. true if setting mode was left. */
static bool Confirm(void)
{
        if (entryCount && !SetField(entryValue)) {
                entryCount = 0;
                Beep_Error();
                return false;
        }
        return NextField();
}

/* A digit key. true if setting mode was left. */
static bool Digit(uint8_t d)
{
        if (entryCount == 0) {
                entryValue = d;
                entryCount = 1;
                return false;
        }
        entryCount = 0;
        if (!SetField((uint8_t)(entryValue * 10 + d))) {
                Beep_Error();
                return false;
        }
        return NextField();
}

/* ---------- public ---------- */

void Clock_Init(void)
{
        Buttons_Init();
        CorrLoad();
        RTCCAL = 0;                     // no hardware calibration, see CorrTick()
        state = CLK_TIME;
        lastSec = -1;
        corrLastSec = -1;
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
                if (!RTCC_TimeGet(&now)) return;        // RTCC registers are updating
                if (CorrTick(&now)) {
                        RTCC_TimeGet(&now);
                        lastSec = -1;                   // seconds moved: redraw
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
        bool input = pressed || key;
        if (input) {
                idleTicks = 0;
                ticks = 0;                      // show the field right after a press
        } else if (++idleTicks >= SET_TIMEOUT_TICKS) {
                LeaveSetting();
                return;
        }

        bool left = false;
        if (state == CLK_SET_COR && !(pressed & BTN_PRGM) && key != '=' && key != '.') {
                CorrInput(pressed, key);
                if (Buttons_Held() & BTN_MODE || Keypad_Held() == '+' || Keypad_Held() == '-')
                        input = true;                   // keep the value lit and redrawn while held
        } else if ((pressed & BTN_PRGM) || key == '=') {
                left = Confirm();
        } else if ((pressed & BTN_MODE) || key == '+') {
                Step(+1);
        } else if (key == '-') {
                Step(-1);
        } else if (key >= '0' && key <= '9') {
                left = Digit((uint8_t)(key - '0'));
        } else if (key == '.') {
                LeaveSetting();
                left = true;
        }
        if (left) return;                       // the time screen redraws itself

        // Redraw only when something changes: a press or a blink phase switch.
        // The field doesn't blink while MODE is held or a digit is pending.
        if (input || ticks == 0 || ticks == BLINK_ON_TICKS) {
                bool hide = (ticks >= BLINK_ON_TICKS) && !(Buttons_Held() & BTN_MODE) && !Keypad_Held() && !entryCount;
                Render(&edit, state, hide ? state : CLK_TIME, true);
        }
        if (++ticks >= BLINK_PERIOD_TICKS) ticks = 0;
}
