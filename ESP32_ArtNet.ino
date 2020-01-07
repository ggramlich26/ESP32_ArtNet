/*
This example will receive multiple universes via Artnet and control a strip of ws2811 leds via
Adafruit's NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include "ArtnetWifi.h"
#include "dmx_decoder.h"
#include "effect_generator.h"
#include "dmx_encoder.h"


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

#define	DMX_MODE			ws2812

#define LED_UPDATE_INTERVAL 50
#define DMX_UPDATE_INTERVAL 100

#define ws2812_pin 	23    // Number of the data out pin

uint16_t getDmxAddress();
uint16_t getDmxUniverse();
uint8_t getStripMode();
uint16_t getEffectMode();
void updateEffectMode();
void sendDMXValues();			// Function used to send the current settings in standalone mode via ArtNet

void initWifi();
void initTest();

#define ADDR_SWITCH		0b10000000
#define UNIVERSE_SWITCH	0b01110011

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											Effect Mode														//
//																											//
//						Meaning of the DIP switches															//
//																											//
//								ADDR Switches																//
//		-------------------------------------------------------------------------							//
//		| ADDR_1 | ADDR_2 | ADDR_3 | ADDR_4 | ADDR_5 | ADDR_6 | ADDR_7 | ADDR_8 |							//
//		-------------------------------------------------------------------------							//
//			0		 1		  2		   3		4		 5		  6		   7								//
//																											//
//									Universe Switches														//
//		----------------------------------------------------------------									//
//		| ADDR_9 | UNI_1 | UNI_2 | UNI_3 | UNI_4 | UNI_5 | UNI_6 | MODE |									//
//		----------------------------------------------------------------									//
//			0	     1	     2	     3		 4		 5		 6	    7										//
//																											//
//		MODE: ArtNet vs. stand-alone																		//
//				0: ArtNet; 1: Stand-alone mode																//
//		ArtNet mode: 																						//
//			ADDR_1 - ADDR_9: DMX address																	//
//			UNI_1 - UNI_6: ArtNet Universe																	//
//		Stand-alone mode:																					//
//			UNI_6:	0: Local only; 1: Master																//
//			ADDR_1 - ADDR_3: Red color																		//
//			ADDR_4 - ADDR_6: Green color																	//
//			ADDR_7 - ADDR_9: Blue color																		//
//			UNI_1 - UNI_5: Effect																			//
//				00000: Steady color																			//
//				100xx:	Blurr forward																		//
//				010xx:	Blurr forward backward																//
//				110xx:	Tear forward 																		//
//				001xx:	Tear forward backward																//
//				101xx:	Fade all																			//
//				011xx:	Stars																				//
//				111xx: Random effects																		//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//DMX Definitions
#define NDMXVALUES  9      //Number of DMX Values to be recieved starting from the startaddress

const char* ssid     = "Zeus Lighting";
const char* password = "BeautyOfLight";
#define HOST_NAME		"Light Tube 1"

uint8_t wifi_initialized = 0;
uint8_t wifi_ap = 0;

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 1; // some software send out artnet first universe as 0.
const char host[] = "192.168.4.255"; // CHANGE FOR YOUR SETUP your destination

byte broadcast[] = {192, 168, 178, 255};
void setup()
{
//  nvs_flash_init();
  ws2812_init(ws2812_pin);
  pinMode(2, OUTPUT);

  Serial.begin(115200);

  if(!(getEffectMode()>>15) || (getEffectMode()>>14) == 0x03){
	  initWifi();
  }

  initTest();

	efg_init();
}

void loop()
{
	//init Wifi if swithing to ArtNet mode
	if((!(getEffectMode() >> 15 || (getEffectMode()>>14) == 0x03)) && !wifi_initialized){
		initWifi();
	}
	updateEffectMode();

  if(!(getEffectMode()>>15)){
	  // we call the read function inside the loop
	  artnet.read();
  }
  else if((getEffectMode()>>14) == 0x03){
	  static unsigned long lastArtNetUpdateTime = 0;
	  unsigned long time = millis();
	  if(time - lastArtNetUpdateTime >  DMX_UPDATE_INTERVAL){
		  lastArtNetUpdateTime = time;
		  sendDMXValues();
	  }
  }
  else{
	  uint8_t effect = getEffectMode() >> 9;
  }

  static unsigned long lastLedUpdateTime = millis();
  unsigned long time = millis();
  if(time - lastLedUpdateTime >= LED_UPDATE_INTERVAL){
	  efg_inc_time_diff(time - lastLedUpdateTime>0xFF?0xFF:time-lastLedUpdateTime);
	  efg_update();
	  lastLedUpdateTime = time;
	  ws2812_setColors(NUMBER_LEDS, efg_get_data());
  }
}

//void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  static bool bufferFlushed = false;
  static int lastUniverseRead = 0xFFFF;
  static unsigned long lastUpdateTime = 0;
  if(millis()-lastUpdateTime < DMX_UPDATE_INTERVAL){
    artnet.flushBuffer();
    return;
  }
  if(!bufferFlushed){
    artnet.flushBuffer();
    bufferFlushed = true;
    return;
  }
//  if((int)universe == lastUniverseRead){
//    lastUniverseRead = 0xFFFF;
//    bufferFlushed = false;
//    lastUpdateTime = millis();
//    return;
//  }
  if(universe == getDmxUniverse()){
    bufferFlushed = false;
    lastUpdateTime = millis();
  }
  else{
	  return;
  }
  lastUniverseRead = universe;
  //read dmx values
  uint8_t dmxValues[NDMXVALUES];
  for(int i = getDmxAddress()-1, j=0; j < NDMXVALUES; i++, j++){
	  dmxValues[j] = data[i];
  }

//  for(uint8_t i = 0; i < NDMXVALUES; i++){
//	  Serial.print(String(dmxValues[i]) + ", ");
//  }
//  Serial.println();
//
  dmx_decode(dmxValues, getStripMode());
}

void onSync(IPAddress remoteIP) {
}

void sendDMXValues(){
	dmx_encode();
	for(uint8_t i = 0; i < NDMXVALUES; i++){
		artnet.setByte(i, dmx_encode_get_data(i));
	}
	artnet.write();
}

void initTest()
{
	uint8_t data[3*NUMBER_LEDS];
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0xFF;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0xFF;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
}

void initTestBlue()
{
	uint8_t data[3*NUMBER_LEDS];
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0xFF;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0xFF;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(100);
	for(int i = 0; i < NUMBER_LEDS*3;){
		data[i++] = 0x00;
		data[i++] = 0x00;
		data[i++] = 0x00;
	}
	ws2812_setColors(NUMBER_LEDS, &(data[0]));
	delay(300);
}

//returns DMX Address. This is the value from the address DIP switches in inversed order
uint16_t getDmxAddress(){
	return getEffectMode() & 0x01FF;
}

uint8_t getStripMode(){
	return DMX_MODE;
}

//returns artNet universe. This is the value from the universe DIP switches in inversed order
uint16_t getDmxUniverse(){
	return (getEffectMode() >> 9) & 0x7F;
}

uint8_t invert_bitorder(uint8_t b){
	   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	   return b;
}
uint16_t getEffectMode(){
	return invert_bitorder(ADDR_SWITCH) | (invert_bitorder(UNIVERSE_SWITCH) << 8);
}

void updateEffectMode(){
	static unsigned long nextEffectUpdateTime = 0;
	static uint32_t lastEffectMode = 0x00000000;

	uint8_t effect = getEffectMode() >> 9;
	effect = effect & 0b00011111;

	if(getEffectMode() != lastEffectMode){
		lastEffectMode = getEffectMode();

		uint8_t r = (getEffectMode()<<5) & 0xE0;
		if(r)
			r |= 0x1F;
		uint8_t g = (getEffectMode()<<2) & 0xE0;
		if(g)
			g |= 0x1F;
		uint8_t b = (getEffectMode()>>1) & 0xE0;
		if(b)
			b |= 0x1F;
		efg_color_set_steady(r, g, b);
		//steady
		if(effect == 0x00){
			efg_set_steady();
		}
		//Blurr forward
		else if((effect&0x07) == 0b001){
			switch(effect>>3){
			case 0:
				efg_set_blurr(m_chase_blurr_f, 1, 20, 100, 2);
				break;
			case 1:
				efg_set_blurr(m_chase_blurr_f, 1, 20, 400, 2);
				break;
			case 2:
				efg_set_blurr(m_chase_blurr_f, 1, 105, 100, 2);
				break;
			case 3:
				efg_set_blurr(m_chase_blurr_f, NUMBER_LEDS, 100, 100, 0);
				break;
			}

		}
		//Blurr forward backward
		else if((effect&0x07) == 0b010){

			switch(effect>>3){
			case 0:
				efg_set_blurr(m_chase_blurr_fb, 1, 20, 100, 2);
				break;
			case 1:
				efg_set_blurr(m_chase_blurr_fb, 1, 20, 400, 2);
				break;
			case 2:
				efg_set_blurr(m_chase_blurr_fb, 1, 105, 100, 2);
				break;
			case 3:
				efg_set_blurr(m_chase_blurr_fb, 1, 105, 400, 2);
				break;
			}
		}
		//Tear forward
		else if((effect&0x07) == 0b011){

			switch(effect>>3){
			case 0:
				efg_set_blurr(m_tear_f, 1, 20, 100, 2);
				break;
			case 1:
				efg_set_blurr(m_tear_f, 1, 20, 400, 2);
				break;
			case 2:
				efg_set_blurr(m_tear_f, 1, 105, 100, 2);
				break;
			case 3:
				efg_set_blurr(m_tear_f, 2, 105, 100, NUMBER_LEDS/2);
				break;
			}
		}
		//Tear forward backward
		else if((effect&0x07) == 0b100){

			switch(effect>>3){
			case 0:
				efg_set_blurr(m_tear_fb, 1, 20, 100, 3);
				break;
			case 1:
				efg_set_blurr(m_tear_fb, 1, 20, 400, 3);
				break;
			case 2:
				efg_set_blurr(m_tear_fb, 2, 105, 100, NUMBER_LEDS/2);
				break;
			case 3:
				efg_set_blurr(m_tear_fb, 2, 105, 400, NUMBER_LEDS/2);
				break;
			}
		}
		//Fade all
		else if((effect&0x07) == 0b101){
			efg_set_steady();

			switch(effect>>3){
			case 0:
				efg_color_set_fade_switch(c_fade_all, 100, 255);
				break;
			case 1:
				efg_color_set_fade_switch(c_fade_all, 400, 255);
				break;
			case 2:
				efg_color_set_fade_switch(c_switch_all, 100, 255);
				break;
			case 3:
				efg_color_set_fade_switch(c_switch_all, 400, 255);
				break;
			}
		}
		//Stars
		else if((effect&0x07) == 0b110){
			switch(effect>>3){
			case 0:
				efg_set_stars(10, 50);
				break;
			case 1:
				efg_set_stars(50, 100);
				break;
			case 2:
				efg_set_snow(10,50);
				break;
			case 3:
				efg_set_snow(10,50);
				efg_color_set_snow_rainbow(255, 100);
				break;
			}
		}
		//Random effects
		else if((effect&0x07) == 0b111){
			nextEffectUpdateTime = 0;
		}
		efg_normalize_values();
	}
	//Random effects
	if((effect&0x07) == 0b111 && millis() >= nextEffectUpdateTime){
		static uint8_t effect_step = 0;
		if(nextEffectUpdateTime == 0){
			effect_step = 0;
		}
		switch(effect>>3){
		case 0:
			switch(effect_step){
			case 0:
				efg_set_blurr(m_tear_fb, 1, 20, 100, 3);
				efg_color_set_steady(255, 0, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 1:
				efg_set_blurr(m_chase_blurr_f, 1, 20, 100, 2);
				efg_color_set_steady(255, 150, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 2:
				efg_set_blurr(m_chase_blurr_fb, 3, 110, 100, 3);
				efg_color_set_steady(255, 0, 255);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 3:
				efg_set_blurr(m_tear_f, 2, 25, 100, 4);
				efg_color_set_rainbow(255, 100);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 4:
				efg_set_blurr(m_chase_blurr_f, NUMBER_LEDS, 100, 100, 0);
				efg_color_set_steady(0, 0, 255);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 5:
				efg_set_blurr(m_chase_blurr_f, 1, 25, 100, 2);
				efg_color_set_rainbow_chase(c_rainbow_chase_f, 100, 255, 100);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 6:
				efg_set_blurr(m_tear_fb, 1, 110, 100, 10);
				efg_color_set_steady(0, 255, 255);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 7:
				efg_set_steady();
				efg_color_set_fade_switch(c_fade_all, 100, 255);
				nextEffectUpdateTime = millis() + 30000;
				break;
			}
			effect_step = (effect_step + 1) % 8;
			break;
		case 1:
		{
			uint8_t r = (getEffectMode()<<5) & 0xE0;
			if(r)
				r |= 0x1F;
			uint8_t g = (getEffectMode()<<2) & 0xE0;
			if(g)
				g |= 0x1F;
			uint8_t b = (getEffectMode()>>1) & 0xE0;
			if(b)
				b |= 0x1F;
			efg_color_set_steady(r, g, b);
			switch(effect_step){
			case 0:
				efg_set_blurr(m_tear_fb, 1, 20, 100, 3);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 1:
				efg_set_blurr(m_chase_blurr_f, 1, 20, 100, 2);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 2:
				efg_set_blurr(m_chase_blurr_fb, 3, 110, 100, 3);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 3:
				efg_set_blurr(m_tear_f, 2, 25, 100, 4);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 4:
				efg_set_blurr(m_chase_blurr_f, NUMBER_LEDS, 100, 100, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 5:
				efg_set_blurr(m_tear_fb, 1, 110, 100, 10);
				nextEffectUpdateTime = millis() + 60000;
				break;
			}
			effect_step = (effect_step + 1) % 6;
			break;
		}
		case 2:
			switch(effect_step){
			case 0:
				efg_set_blurr(m_tear_f, 2, 30, 400, 20);
				efg_color_set_steady(255, 110, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 1:
				efg_set_blurr(m_tear_f, 2, 30, 400, 4);
				efg_color_set_steady(255, 0, 15);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 2:
				efg_set_blurr(m_chase_blurr_f, 2, 60, 400, 10);
				efg_color_set_steady(255, 30, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 3:
				efg_set_blurr(m_chase_blurr_f, 2, 60, 400, 10);
				efg_color_set_steady(255, 60, 0);
				nextEffectUpdateTime = millis() + 60000;
				break;
			case 4:
				efg_set_steady();
				efg_color_set_rainbow_chase(c_rainbow_chase_f, 600, 100, 100);
				break;
			}
			effect_step = (effect_step + 1) % 5;
			break;
		case 3:
			//not implemented
			efg_color_set_steady(0, 0, 0);	//todo: remove this line when implementing function
			switch(effect_step){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			}
			effect_step = (effect_step + 1) % 6;
			break;
		}
		efg_normalize_values();
	}
}

void initWifi(){
  if(!(getEffectMode()>>15)){
	  WiFi.begin(ssid, password);
	  WiFi.setHostname(HOST_NAME);
	  while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		Serial.print(".");
	  }
	  Serial.println("");
	  Serial.print("Connected to ");
	  Serial.println(ssid);
	  Serial.print("IP address: ");
	  Serial.println(WiFi.localIP());

	  artnet.begin();

	  // this will be called for each packet received
	  artnet.setArtDmxCallback(onDmxFrame);
	  wifi_initialized = 1;
	  wifi_ap = 0;
	  initTestBlue();
  }
  else{
	  WiFi.softAP(ssid, password);
	  IPAddress IP = WiFi.softAPIP();
	  Serial.print("AP IP address: ");
	  Serial.println(IP);

	  artnet.begin(host);
	  artnet.setLength(NDMXVALUES);
	  artnet.setUniverse(startUniverse);

	  wifi_initialized = 1;
	  wifi_ap = 1;
	  initTestBlue();
	  initTestBlue();
  }
}

#define ETS_RMT_CTRL_INUM	18
#define ESP_RMT_CTRL_DISABLE	ESP_RMT_CTRL_DIABLE /* Typo in esp_intr.h */

#define DIVIDER		4 /* Above 4, timings start to deviate*/
#define DURATION	12.5 /* minimum time of a single RMT duration
				in nanoseconds based on clock */

#define PULSE_T0H	(  350 / (DURATION * DIVIDER));
#define PULSE_T1H	(  900 / (DURATION * DIVIDER));
#define PULSE_T0L	(  900 / (DURATION * DIVIDER));
#define PULSE_T1L	(  350 / (DURATION * DIVIDER));
#define PULSE_TRS	(50000 / (DURATION * DIVIDER));

#define MAX_PULSES	32

#define RMTCHANNEL	0

typedef union {
  struct {
    uint32_t duration0:15;
    uint32_t level0:1;
    uint32_t duration1:15;
    uint32_t level1:1;
  };
  uint32_t val;
} rmtPulsePair;

static uint8_t *ws2812_buffer = NULL;
static unsigned int ws2812_pos, ws2812_len, ws2812_half;
static xSemaphoreHandle ws2812_sem = NULL;
static intr_handle_t rmt_intr_handle = NULL;
static rmtPulsePair ws2812_bits[2];

void ws2812_initRMTChannel(int rmtChannel)
{
  RMT.apb_conf.fifo_mask = 1;  //enable memory access, instead of FIFO mode.
  RMT.apb_conf.mem_tx_wrap_en = 1; //wrap around when hitting end of buffer
  RMT.conf_ch[rmtChannel].conf0.div_cnt = DIVIDER;
  RMT.conf_ch[rmtChannel].conf0.mem_size = 1;
  RMT.conf_ch[rmtChannel].conf0.carrier_en = 0;
  RMT.conf_ch[rmtChannel].conf0.carrier_out_lv = 1;
  RMT.conf_ch[rmtChannel].conf0.mem_pd = 0;

  RMT.conf_ch[rmtChannel].conf1.rx_en = 0;
  RMT.conf_ch[rmtChannel].conf1.mem_owner = 0;
  RMT.conf_ch[rmtChannel].conf1.tx_conti_mode = 0;    //loop back mode.
  RMT.conf_ch[rmtChannel].conf1.ref_always_on = 1;    // use apb clock: 80M
  RMT.conf_ch[rmtChannel].conf1.idle_out_en = 1;
  RMT.conf_ch[rmtChannel].conf1.idle_out_lv = 0;

  return;
}

void ws2812_copy()
{
  unsigned int i, j, offset, len, bit;


  offset = ws2812_half * MAX_PULSES;
  ws2812_half = !ws2812_half;

  len = ws2812_len - ws2812_pos;
  if (len > (MAX_PULSES / 8))
    len = (MAX_PULSES / 8);

  if (!len) {
    for (i = 0; i < MAX_PULSES; i++)
      RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;
    return;
  }

  for (i = 0; i < len; i++) {
    bit = ws2812_buffer[i + ws2812_pos];
    for (j = 0; j < 8; j++, bit <<= 1) {
      RMTMEM.chan[RMTCHANNEL].data32[j + i * 8 + offset].val =
	ws2812_bits[(bit >> 7) & 0x01].val;
    }
    if (i + ws2812_pos == ws2812_len - 1)
      RMTMEM.chan[RMTCHANNEL].data32[7 + i * 8 + offset].duration1 = PULSE_TRS;
  }

  for (i *= 8; i < MAX_PULSES; i++)
    RMTMEM.chan[RMTCHANNEL].data32[i + offset].val = 0;

  ws2812_pos += len;
  return;
}

void ws2812_handleInterrupt(void *arg)
{
  portBASE_TYPE taskAwoken = 0;


  if (RMT.int_st.ch0_tx_thr_event) {
    ws2812_copy();
    RMT.int_clr.ch0_tx_thr_event = 1;
  }
  else if (RMT.int_st.ch0_tx_end && ws2812_sem) {
    xSemaphoreGiveFromISR(ws2812_sem, &taskAwoken);
    RMT.int_clr.ch0_tx_end = 1;
  }

  return;
}

void ws2812_init(int gpioNum)
{
  DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_RMT_CLK_EN);
  DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_RMT_RST);

  rmt_set_pin((rmt_channel_t)RMTCHANNEL, RMT_MODE_TX, (gpio_num_t)gpioNum);

  ws2812_initRMTChannel(RMTCHANNEL);

  RMT.tx_lim_ch[RMTCHANNEL].limit = MAX_PULSES;
  RMT.int_ena.ch0_tx_thr_event = 1;
  RMT.int_ena.ch0_tx_end = 1;

  ws2812_bits[0].level0 = 1;
  ws2812_bits[0].level1 = 0;
  ws2812_bits[0].duration0 = PULSE_T0H;
  ws2812_bits[0].duration1 = PULSE_T0L;
  ws2812_bits[1].level0 = 1;
  ws2812_bits[1].level1 = 0;
  ws2812_bits[1].duration0 = PULSE_T1H;
  ws2812_bits[1].duration1 = PULSE_T1L;

  esp_intr_alloc(ETS_RMT_INTR_SOURCE, 0, ws2812_handleInterrupt, NULL, &rmt_intr_handle);

  return;
}

//void ws2812_setColors(unsigned int length, rgbVal *array)
//{
//  unsigned int i;
//
//
//  ws2812_len = (length * 3) * sizeof(uint8_t);
//  ws2812_buffer = (uint8_t*)(malloc(ws2812_len));
//
//  for (i = 0; i < length; i++) {
//    ws2812_buffer[0 + i * 3] = array[i].g;
//    ws2812_buffer[1 + i * 3] = array[i].r;
//    ws2812_buffer[2 + i * 3] = array[i].b;
//  }
//
//  ws2812_pos = 0;
//  ws2812_half = 0;
//
//  ws2812_copy();
//
//  if (ws2812_pos < ws2812_len)
//    ws2812_copy();
//
//  ws2812_sem = xSemaphoreCreateBinary();
//
//  RMT.conf_ch[RMTCHANNEL].conf1.mem_rd_rst = 1;
//  RMT.conf_ch[RMTCHANNEL].conf1.tx_start = 1;
//
//  xSemaphoreTake(ws2812_sem, portMAX_DELAY);
//  vSemaphoreDelete(ws2812_sem);
//  ws2812_sem = NULL;
//
//  free(ws2812_buffer);
//
//  return;
//}

void ws2812_setColors(unsigned int length, uint8_t *array)
{
  unsigned int i;


  ws2812_len = (length * 3) * sizeof(uint8_t);
//  ws2812_buffer = (uint8_t*)(malloc(ws2812_len));
//
//  for (i = 0; i < length; i++) {
//    ws2812_buffer[0 + i * 3] = array[i].g;
//    ws2812_buffer[1 + i * 3] = array[i].r;
//    ws2812_buffer[2 + i * 3] = array[i].b;
//  }
  ws2812_buffer=array;

  ws2812_pos = 0;
  ws2812_half = 0;

  ws2812_copy();

  if (ws2812_pos < ws2812_len)
    ws2812_copy();

  ws2812_sem = xSemaphoreCreateBinary();

  RMT.conf_ch[RMTCHANNEL].conf1.mem_rd_rst = 1;
  RMT.conf_ch[RMTCHANNEL].conf1.tx_start = 1;

  xSemaphoreTake(ws2812_sem, portMAX_DELAY);
  vSemaphoreDelete(ws2812_sem);
  ws2812_sem = NULL;

//  free(ws2812_buffer);

  return;
}
