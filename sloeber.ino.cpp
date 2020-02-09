#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2020-02-09 21:54:05

#include "Arduino.h"
#include "ArtnetWifi.h"
#include "dmx_decoder.h"
#include "effect_generator.h"
#include "dmx_encoder.h"
#include "config.h"
#include "ws2812.h"
#include "webserver.h"
#include "Arduino.h"
#include "EEPROM.h"
#include <stdint.h>

void setup() ;
void loop() ;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) ;
void onSync(IPAddress remoteIP) ;
void sendDMXValues();
void initTest() ;
void initTestBlue() ;
void initSwitches();
uint16_t getDmxAddress();
uint8_t getStripMode();
uint16_t getDmxUniverse();
uint8_t invert_bitorder(uint8_t b);
uint16_t getEffectMode();
void updateEffectMode();
void initWifi();
String changeWifi(const char* currentPassword, const char* newSSID, const char* newPassword);
uint32_t calculateWIFIChecksum();

#include "ESP32_ArtNet.ino"


#endif
