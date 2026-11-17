/**
 *  Main Source File
 *
 * For iCESIO hardware V.1pic16
 *
 *   Replacement board for CASIO 3208 modules (CA-53, CA-506 watches)
 *   MCU: Microchip PIC16F19197 - 56kB flash, 4K RAM, 256b EEPROM
 *   Accelerometer: NXP MMA8451QT - 3-axis, 14-bit/8-bit digital accelerometer
 *   Compass: Honeywell HMC5883L - 3-Axis Digital Compass
 *   Environment: BOSCH BME280 - Digital humidity, pressure and temperature sensor
 *   EEPROM: Microchip 24CW1280T-I/CS0668 - 16KB, WLCSP package
 *   LED: ON Semiconductor NCP5623CMUTBG - Triple Output I2C Controlled RGB LED Driver
 *
 */
#define __VERSION "pic16-0.1"

#define POST_DELAY  100
#define UDELAY 2000

#include "main.h"
#include "mcc_generated_files/mcc.h"
#include "lcd.h"
#include "led.h"
#include "compass.h"
#include "bme280.h"
#include "uart.h"
#include "mma8451.h"
#include "clock.h"
#include "beep.h"
#include "keypad.h"

//#include <stdio.h>
/*
   Main application
 */

#define USE_COMPASS 0   // 1 - init and poll HMC5883L, 0 - compass disabled for now
void main(void)
{
        
        // initialize the device
        SYSTEM_Initialize();

        // Enable the Global Interrupts
        INTERRUPT_GlobalInterruptEnable();

        // Enable the Peripheral Interrupts
        INTERRUPT_PeripheralInterruptEnable();

        // Disable the Global Interrupts
        //INTERRUPT_GlobalInterruptDisable();

        // Disable the Peripheral Interrupts
        //INTERRUPT_PeripheralInterruptDisable();

        // POST Process....

        // Init console
        print("\r\n\n\n\n\niCESIO v." __VERSION"\r\n");
        print("POST: \r\n");
/*        RTCC_SetCallback(RTCC_Callback);
        rtccTimeDate currentTime;
        currentTime.tm_year = 24;    // 2024
        currentTime.tm_mon = 4;      // Апрель
        currentTime.tm_mday = 15;    // 15-е
        currentTime.tm_hour = 14;
        currentTime.tm_min = 0;
        currentTime.tm_sec = 0;
*/
//        RTCC_TimeSet(&currentTime);
  //      RTCC_DateSet(&currentTime);

        // Init LCD
        print("\rLCD...");
        LCD_Initialize();
        ClearDisplay();
        LCD_Enable();
        DisplayPrint(" ICESIO ");
        DrawDisplay();

        // Init RGB LED
        print("\rLED...");
        if (led_init()) {
                led_set(15,0,0);  // RED led while initialize
                print("OK");
        } else print("FAILED\r\n");

#if USE_COMPASS
        // Init Compass
        print("\rCOMPASS...");
        if (HMC5883L_Init()) print("OK");
                        else print("FAILED\r\n");
#endif

        // Init EEPROM
//        print("\rEEPROM...");
        
        // Init Environment
        print("\rENVIR...");
        BME280_Init();
        
        // Init Accelerometer
        print("\rACCEL...");
        if (MMAInit(10)) print("OK");
                    else print("FAILED\r\n");

        // Init RTC
        print("\rRTC...");
        RTCC_Enable();


// POST Completed
        led_set(0,0,0); // All Done
        led_shutdown(); // LED Driver 0to sleep mode

        // Buzzer
        print("\rBEEP...");
        Beep_Init();

        // Clock and side buttons
        Clock_Init();

        print("\rcomplete.\r\n");

        // Keypad last: it may take RG1 (UART TX) over
        Keypad_Init();

// Setup WDT and INTERRUPTS

// Enter LPM
//    SLEEP();

        while(1) {
                /* dots in output -- problem in sleep mode
                 */
//               print(".");
               
#if USE_COMPASS
                HMC5883L_Read();
#endif
 //          printf("C X:%d Y:%d Z:%d\r\n",X_axis,Y_axis,Z_axis);

//              BME280_UpdateValues();
//        printf("T:%d P:%d H:%d\r\n",BME280_Temperature(),BME280_Pressure(),BME280_Humidity());

//        int16_t x,y,z;
  //             MMAGetAccel(&x,&y,&z);
  //             printf("A X:%d Y:%d Z:%d\n\r",x,y,z);

        
// Clock: time display and setting with PRGM / MODE
                Clock_Tick();

                __delay_ms(TICK_MS);
        }

}
/**
   End of File
 */
