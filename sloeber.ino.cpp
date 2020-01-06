#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2020-01-06 15:04:44

#include "Arduino.h"
#include "Artnet_modified.h"
#include "dmx_decoder.h"
#include "effect_generator.h"
#include "Arduino.h"
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <soc/rmt_struct.h>
#include <soc/dport_reg.h>
#include <driver/gpio.h>
#include <soc/gpio_sig_map.h>
#include <esp_intr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <driver/rmt.h>
#include "ws2812.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/rmt_struct.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <stdio.h>
#include "ws2812.h"

void setup() ;
void loop() ;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP) ;
void onSync(IPAddress remoteIP) ;
void initTest() ;
void initTestBlue() ;
uint16_t getDmxAddress();
uint8_t getStripMode();
uint16_t getDmxUniverse();
uint8_t invert_bitorder(uint8_t b);
uint16_t getEffectMode();
void updateEffectMode();
void initWifi();
void ws2812_initRMTChannel(int rmtChannel) ;
void ws2812_copy() ;
void ws2812_handleInterrupt(void *arg) ;
void ws2812_init(int gpioNum) ;
void ws2812_setColors(unsigned int length, uint8_t *array) ;

#include "ESP32_ArtNet.ino"


#endif
