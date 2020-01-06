/*
 * dmx_decoder.c
 *
 *  Created on: Nov 12, 2017
 *      Author: rebotko
 */


#include "dmx_decoder.h"
#include "effect_generator.h"

/**
 * Function that decodes the recieved dmx values and updates the used color and motion.
 */
void dmx_decode(uint8_t *data, uint8_t strip_mode){
	if(strip_mode == ws2812){
		uint8_t motion = data[0];
		switch (motion){
		case M_STEADY:
			efg_set_steady();
			break;
		case M_CHASE_F:
			//time calculation:
			//t=1/(1/t_max+1/t_min*x/255), where x is a value from 1 to 255
			//this makes the frequency grow linear with x
			efg_set_chase(m_chase_f, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0);
			break;
		case M_CHASE_B:
			efg_set_chase(m_chase_b, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0);
			break;
		case M_CHASE_CROSS:
			efg_set_chase(m_chase_cross, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0);
			break;
		case M_CHASE_MEET:
			efg_set_chase(m_chase_meet, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0);
			break;
		case M_CHASE_BLURR_F:
			efg_set_blurr(m_chase_blurr_f, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0, data[4]);
			break;
		case M_CHASE_BLURR_FB:
			efg_set_blurr(m_chase_blurr_fb, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0, data[4]);
			break;
		case M_TEAR_F:
			efg_set_blurr(m_tear_f, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0, data[4]);
			break;
		case M_TEAR_FB:
			efg_set_blurr(m_tear_fb, data[2], data[3], data[1]?1/(1/1021.0+1/3.0*(data[1]-1)/255.0):0, data[4]);
			break;
		case M_FLASH:
			efg_set_flash(m_flash, data[1]?1/(1/5000.0+1/64.0*(data[1]-1)/255.0):0);		//todo: adapt to reasonalbe frequency
			break;
		case M_GLOW:
			efg_set_flash(m_glow, data[1]?1/(1/1250.0+1/1.0*(data[1]-1)/255.0):0);		//todo: adapt to reasonable frequency
			break;
		case M_STARS:
			efg_set_stars(data[1]?(uint16_t)1021-(data[1]<<2):0, data[2]);
			break;
		case M_SEGMENT_4:
			efg_set_segment(m_segment_4, data[1], data[2], data[3], data[4]);
			break;
		case M_SEGMENT_8:
			efg_set_segment(m_segment_8, data[1], data[2], data[3], data[4]);
			break;
		case M_SEGMENT_CUSTOM:
			efg_set_segment_custom(data[1], data[2], data[3], data[4]);
			break;
		case M_EQUALIZER:
			efg_set_segment_custom(0, (uint16_t)data[1]*100/255, 0, 0);
			break;
		case M_EQUALIZER_MIRRORED:
			efg_set_segment_custom(0, (uint16_t)data[1]*100/255/2, 100-(uint16_t)data[1]*100/255/2, (uint16_t)data[1]*100/255/2);
			break;
		case M_SNOW:
			efg_set_snow(data[1], data[2]);
			break;
		}

		uint8_t color = data[5];
		switch(color){
		case C_STEADY:
			if(motion != M_SNOW){
				efg_color_set_steady(data[6], data[7], data[8]);
			}
			else{
				efg_color_set_snow_steady(data[6], data[7], data[8]);
			}
			break;
		case C_FADE_ALL:
			efg_color_set_fade_switch(c_fade_all, data[8]?1/(1/(1250.0)+1/(8.0)*(data[8]-1)/255.0):0, data[6]);
			break;
		case C_SWITCH_ALL:
			efg_color_set_fade_switch(c_switch_all, data[8]?1/(1/(5000.0)+1/(64.0)*(data[8]-1)/255.0):0, data[6]);
			break;
		case C_FADE_3:
			efg_color_set_fade_switch(c_fade_3, data[8]?1/(1/1250.0+1/1.0*(data[8]-1)/255.0):0, data[6]);
			break;
		case C_SWITCH_3:
			efg_color_set_fade_switch(c_switch_3, data[8]?1/(1/(5000.0)+1/(64.0)*(data[8]-1)/255.0):0, data[6]);
			break;
		case C_RAINBOW:
			if(motion != M_SNOW){
				efg_color_set_rainbow(data[6], data[7]);
			}
			else{
				efg_color_set_snow_rainbow(data[6], data[7]);
			}
			break;
		case C_RAINBOW_CHASE_F:
			efg_color_set_rainbow_chase(c_rainbow_chase_f,data[8]?1/(1/1021.0+1/3.0*(data[8]-1)/255.0):0, data[6], data[7]);
			break;
		case C_RAINBOW_CHASE_B:
			efg_color_set_rainbow_chase(c_rainbow_chase_b, data[8]?1/(1/1021.0+1/3.0*(data[8]-1)/255.0):0, data[6], data[7]);
			break;
		}

		efg_normalize_values();
	}
	else if(strip_mode == rgb){
		efg_color_set_steady(data[0], data[1], data[2]);
	}
}
