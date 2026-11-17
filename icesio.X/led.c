/*
   NCP5623C -- Triple Output I2C Controlled RGB LED Driver
 */
#include <xc.h>
#include "mcc_generated_files/mcc.h"

//NCP5623C
#define NCP5623C_ADDRESS     (0x72 / 2) // MCC I2C driver expect 7 bit address.
#define NCP5623C_Shutdown    0b00000000
#define NCP5623C_SetCurrent  0b00100000 // Set ILED current
#define NCP5623C_PWM1        0b01000000 // PWN chan 1
#define NCP5623C_PWM2        0b01100000 // PWN chan 2
#define NCP5623C_PWM3        0b10000000 // PWN chan 3
#define NCP5623C_UPWARD      0b10100000 // Upward
#define NCP5623C_DWNWRD      0b11000000 // Downward
#define NCP5623C_GRADIENT    0b11100000 // Set Up the Gradual Dimming time and run the sequence

// Output Current Programmed Value (ILED = F(Step))
#define NCP5623C_MaxCurrent 9          // 13.8mA

// Step/ILED (mA)
// 0/$00 0       9/$09 1.25   18/$12 2.12   27/$1B 6.90
// 1/$01 0.92   10/$0A 1.31   19/$13 2.30   28/$1C 9.20
// 2/$02 0.95   11/$0B 1.38   20/$14 2.50   29/$1D 13.80
// 3/$03 0.98   12/$0C 1.45   21/$15 2.76   30/$1E 27.60
// 4/$04 1.02   13/$0D 1.53   22/$16 3.06   31/$1F 27.60
// 5/$05 1.06   14/$0E 1.62   23/$17 3.45
// 6/$06 1.10   15/$0F 1.72   24/$18 3.94
// 7/$07 1.15   16/$10 1.84   25/$19 4.60
// 8/$08 1.20   17/$11 1.97   26/$1A 5.52

bool NCP5623C_Write(uint8_t data)
{
        I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
        I2C1_MasterWrite( &data, 1, NCP5623C_ADDRESS, &status);
        while (status == I2C1_MESSAGE_PENDING);
        return status == I2C1_MESSAGE_COMPLETE;
}

bool led_init() {
        if (!NCP5623C_Write(NCP5623C_SetCurrent | NCP5623C_MaxCurrent)) return false;
        NCP5623C_Write(NCP5623C_PWM1);
        NCP5623C_Write(NCP5623C_PWM2);
        NCP5623C_Write(NCP5623C_PWM3);
        return true;
}

void led_shutdown() {
        NCP5623C_Write(NCP5623C_Shutdown);
        return;
}

void led_set(uint8_t R, uint8_t G, uint8_t B) {
        NCP5623C_Write(NCP5623C_PWM3 | R);
        NCP5623C_Write(NCP5623C_PWM2 | G);
        NCP5623C_Write(NCP5623C_PWM1 | B);
        return;
}
