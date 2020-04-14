/*
 * This sketch will control WS2812 or WS2811 LED strips either via ArtNet or in stand alone mode. It is also capable
 * of creating a WIFI access point over which other devices running the same code can be synchronized.
 */

#undef MBEDTLS_CONFIG_FILE
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

#define	DMX_MODE			ws2812

#define LED_UPDATE_INTERVAL 50
#define DMX_UPDATE_INTERVAL 100


void initSwitches();
uint16_t getDmxAddress();
uint16_t getDmxUniverse();
uint8_t getStripMode();
uint16_t getEffectMode();
void updateEffectMode();
void sendDMXValues();			// Function used to send the current settings in stand alone mode via ArtNet

void initWifi();
void initTest();
uint32_t calculateWIFIChecksum();

#ifndef ON_LIGHT_TUBE_PCB
#define ADDR_SWITCH		0b10000000
#define UNIVERSE_SWITCH	0b01000000
#endif

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
//		All switches 0: Reset wifi configuration to default SSID and password								//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//DMX Definitions
#define NDMXVALUES  9      //Number of DMX Values to be recieved starting from the startaddress

#define SSID_MAX_LEN		30
#define SSID_ADDR			0
#define WIFI_PW_MAX_LEN		30
#define WIFI_PW_ADDR		31
#define	HOST_NAME_MAX_LEN	30
#define	HOST_NAME_ADDR		62
#define CHECKSUM_ADDR		93
#define	CHECKSUM_LEN		4
char ssid[SSID_MAX_LEN+1];
char password[WIFI_PW_MAX_LEN+1];
char hostName[HOST_NAME_MAX_LEN+1];

uint8_t wifi_initialized = 0;
uint8_t wifi_ap = 0;
uint8_t schedule_restart = 0;

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 1; 			// some software send out artnet first universe as 0.
const char host[] = "192.168.4.255";	// breadcast address in WIFI access point mode

void setup()
{
	//  nvs_flash_init();
	ws2812_init(ws2812_pin);
	EEPROM.begin(SSID_MAX_LEN+1+WIFI_PW_MAX_LEN+1+HOST_NAME_MAX_LEN+1+CHECKSUM_LEN);

	Serial.begin(115200);

#ifdef ON_LIGHT_TUBE_PCB
	initSwitches();
#endif

	uint8_t ssid_initialized = 0;
	//read SSID and password from flash
	for(uint8_t i = 0; i < SSID_MAX_LEN+1; i++){
		ssid[i] = EEPROM.read(SSID_ADDR+i);
		if(ssid[i] != 0x00){
			ssid_initialized = 0xFF;
		}
	}
	for(uint8_t i = 0; i < WIFI_PW_MAX_LEN+1; i++){
		password[i] = EEPROM.read(WIFI_PW_ADDR+i);
	}
	for(uint8_t i = 0; i < HOST_NAME_MAX_LEN+1; i++){
		hostName[i] = EEPROM.read(HOST_NAME_ADDR+i);
	}
	uint32_t checksum = 0;
	for(uint8_t i = 0; i < CHECKSUM_LEN; i++){
		checksum |= ((uint32_t)EEPROM.read(CHECKSUM_ADDR+i))<<(8*i);
	}

	if(getEffectMode() == 0x0000 || checksum != calculateWIFIChecksum()){
		//copy default SSID to EEPROM and to the ssid variable
		const char* default_ssid = DEFAULT_SSID;
		for(uint8_t i = 0; i < SSID_MAX_LEN+1; i++){
			EEPROM.write(SSID_ADDR + i, default_ssid[i]);
			ssid[i] = default_ssid[i];
			if(default_ssid[i] == '\0'){
				break;
			}
		}
		//copy default password to EEPROM and to the password variable
		const char* default_pw = DEFAULT_WIFI_PW;
		for(uint8_t i = 0; i < WIFI_PW_MAX_LEN+1; i++){
			EEPROM.write(WIFI_PW_ADDR + i, default_pw[i]);
			password[i] = default_pw[i];
			if(default_pw[i] == '\0'){
				break;
			}
		}
		//copy default host name to EEPROM and to the hostName variable
		const char* default_host_name = DEFAULT_HOST_NAME;
		for(uint8_t i = 0; i < HOST_NAME_MAX_LEN+1; i++){
			EEPROM.write(HOST_NAME_ADDR + i, default_host_name[i]);
			hostName[i] = default_host_name[i];
			if(default_host_name[i] == '\0'){
				break;
			}
		}
		checksum = calculateWIFIChecksum();
		for(uint8_t i = 0; i < CHECKSUM_LEN; i++){
			EEPROM.write(CHECKSUM_ADDR+i, (uint8_t)(checksum>>(8*i)));
		}
		EEPROM.commit();
	}

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
	if(getEffectMode() >> 15){
		updateEffectMode();
	}

	if(!(getEffectMode()>>15)){	//ArtNet mode
		artnet.read();
	}
	else if((getEffectMode()>>14) == 0x03){	//standalone mode with access point
		static unsigned long lastArtNetUpdateTime = 0;
		unsigned long time = millis();
		if(time - lastArtNetUpdateTime >  DMX_UPDATE_INTERVAL){
			lastArtNetUpdateTime = time;
			sendDMXValues();
		}
	}

	//update effect and LEDs
	static unsigned long lastLedUpdateTime = millis();
	unsigned long time = millis();
	if(time - lastLedUpdateTime >= LED_UPDATE_INTERVAL){
		efg_inc_time_diff(time - lastLedUpdateTime>0xFF?0xFF:time-lastLedUpdateTime);
		efg_update();
		lastLedUpdateTime = time;
		ws2812_setColors(NUMBER_LEDS, efg_get_data());
	}

	if(schedule_restart){
		schedule_restart = 0;
		delay(1000);
		ESP.restart();
	}
}

//void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
	static bool bufferFlushed = false;
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
	if(universe == getDmxUniverse()){
		bufferFlushed = false;
		lastUpdateTime = millis();
	}
	else{
		return;
	}
	//read dmx values
	uint8_t dmxValues[NDMXVALUES];
	for(int i = getDmxAddress()-1, j=0; j < NDMXVALUES; i++, j++){
		dmxValues[j] = data[i];
	}
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

//Sets the switch pins to input and enables the internal pullup resistors
void initSwitches(){
	pinMode(5, INPUT_PULLUP);
	pinMode(18, INPUT_PULLUP);
	pinMode(19, INPUT_PULLUP);
	pinMode(21, INPUT_PULLUP);
	pinMode(22, INPUT_PULLUP);
	pinMode(23, INPUT_PULLUP);
	pinMode(13, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);

	pinMode(14, INPUT_PULLUP);
	pinMode(27, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	pinMode(25, INPUT_PULLUP);
	pinMode(33, INPUT_PULLUP);
	pinMode(32, INPUT_PULLUP);
	pinMode(35, INPUT_PULLUP);
	pinMode(34, INPUT_PULLUP);
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
#ifdef ON_LIGHT_TUBE_PCB
	uint16_t res = 0;
	if(!digitalRead(23))
		res |= (1<<0);
	if(!digitalRead(22))
		res |= (1<<1);
	if(!digitalRead(21))
		res |= (1<<2);
	if(!digitalRead(19))
		res |= (1<<3);
	if(!digitalRead(18))
		res |= (1<<4);
	if(!digitalRead(5))
		res |= (1<<5);
	if(!digitalRead(34))
		res |= (1<<6);
	if(!digitalRead(35))
		res |= (1<<7);

	if(!digitalRead(32))
		res |= (1<<8);
	if(!digitalRead(33))
		res |= (1<<9);
	if(!digitalRead(25))
		res |= (1<<10);
	if(!digitalRead(26))
		res |= (1<<11);
	if(!digitalRead(27))
		res |= (1<<12);
	if(!digitalRead(14))
		res |= (1<<13);
	if(!digitalRead(12))
		res |= (1<<14);
	if(!digitalRead(13))
		res |= (1<<15);

	return res;
#else
	return invert_bitorder(ADDR_SWITCH) | (invert_bitorder(UNIVERSE_SWITCH) << 8);
#endif
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
		WiFi.setHostname(hostName);
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
	webserver_init();
}

String changeWifi(const char* currentPassword, const char* newSSID, const char* newPassword, const char* newHostName){
	if(currentPassword == NULL || strcmp(currentPassword, password) != 0){
		return "Setting WIFI credentials failed. Wrong password.";
	}
	if(newSSID != NULL && strcmp(newSSID, "") != 0){
		for(uint8_t i = 0; i < SSID_MAX_LEN+1; i++){
			EEPROM.write(SSID_ADDR + i, newSSID[i]);
			ssid[i] = newSSID[i];
			if(newSSID[i] == '\0'){
				break;
			}
		}
	}
	if(newPassword != NULL && strcmp(newPassword, "") != 0){
		for(uint8_t i = 0; i < WIFI_PW_MAX_LEN+1; i++){
			EEPROM.write(WIFI_PW_ADDR + i, newPassword[i]);
			password[i] = newPassword[i];
			if(newPassword[i] == '\0'){
				break;
			}
		}
	}
	if(newHostName != NULL && strcmp(newHostName, "") != 0){
		for(uint8_t i = 0; i < HOST_NAME_MAX_LEN+1; i++){
			EEPROM.write(HOST_NAME_ADDR + i, newHostName[i]);
			hostName[i] = newHostName[i];
			if(newHostName[i] == '\0'){
				break;
			}
		}
	}
	uint32_t checksum = calculateWIFIChecksum();
	for(uint8_t i = 0; i < CHECKSUM_LEN; i++){
		EEPROM.write(CHECKSUM_ADDR+i, (uint8_t)(checksum>>(8*i)));
	}
	EEPROM.commit();
	schedule_restart = 0xFF;
	return "Successfully set new WIFI credentials. You can reset the wifi credentials to their default values by setting "
			"all address and universe switches to 0 and restarting the device.";
}

uint32_t calculateWIFIChecksum(){
	uint32_t res = 0;
	for(uint8_t i = 0, j = 0; i < SSID_MAX_LEN+1; i++, j = (j+1)%CHECKSUM_LEN){
		res ^= ((uint32_t)(ssid[i])<<(8*j));
		if(ssid[i] == '\0'){
			break;
		}
	}
	for(uint8_t i = 0, j = 0; i < WIFI_PW_MAX_LEN+1; i++, j = (j+1)%CHECKSUM_LEN){
		res ^= ((uint32_t)(password[i])<<(8*j));
		if(password[i] == '\0'){
			break;
		}
	}
	for(uint8_t i = 0, j = 0; i < HOST_NAME_MAX_LEN+1; i++, j = (j+1)%CHECKSUM_LEN){
		res ^= ((uint32_t)(hostName[i])<<(8*j));
		if(hostName[i] == '\0'){
			break;
		}
	}
	return res;
}
