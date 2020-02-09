/* Created 19 Nov 2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This is a driver for the WS2812 RGB LEDs using the RMT peripheral on the ESP32.
 *
 * This code is placed in the public domain (or CC0 licensed, at your option).
 *
 * Adapted by ggramlich26 09. Feb 2020
 */

#ifndef WS2812_DRIVER_H
#define WS2812_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ws2812_init(int gpioNum);
extern void ws2812_setColors(unsigned int length, uint8_t *array);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* WS2812_DRIVER_H */
