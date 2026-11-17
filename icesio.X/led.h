/*
 * File:   led.h
 * Author: ice
 *
 * Created on January 14, 2019, 5:14 PM
 */
#include <stdint.h>
#include <stdbool.h>


extern bool led_init(void);
extern void led_shutdown(void);
extern void led_set(uint8_t R, uint8_t G, uint8_t B);
