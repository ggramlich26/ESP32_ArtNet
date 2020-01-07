/*
 * dmx_encoder.c
 *
 *  Created on: Jan 7, 2020
 *      Author: tsugua
 */


#include "dmx_encoder.h"
#include "effect_generator.h"
#include "dmx_decoder.h"

#define NDMXVALUES	9

uint8_t data[NDMXVALUES];

void dmx_encode(){
	uint8_t motion = efg_get_current_motion();
	switch(motion){
	case m_steady:
		data[0] = M_STEADY;
		break;
	case m_chase_f:
		data[0] = M_CHASE_F;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		break;
	case m_chase_b:
		data[0] = M_CHASE_B;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		break;
	case m_chase_cross:
		data[0] = M_CHASE_CROSS;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		break;
	case m_chase_meet:
		data[0] = M_CHASE_MEET;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		break;
	case m_chase_blurr_f:
		data[0] = M_CHASE_BLURR_F;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		data[4] = efg_get_blur_width()/SPEED_FACTOR;
		break;
	case m_chase_blurr_fb:
		data[0] = M_CHASE_BLURR_FB;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		data[4] = efg_get_blur_width()/SPEED_FACTOR;
		break;
	case m_tear_f:
		data[0] = M_TEAR_F;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		data[4] = efg_get_blur_width()/SPEED_FACTOR;
		break;
	case m_tear_fb:
		data[0] = M_TEAR_FB;
		data[2] = efg_get_segment_width()/SPEED_FACTOR;
		data[3] = efg_get_segment_dist();
		data[1] = efg_get_update_interval()?(765+256.0/1021)/efg_get_update_interval()/SPEED_FACTOR:0;
		data[4] = efg_get_blur_width()/SPEED_FACTOR;
		break;
	case m_flash:
		data[0] = M_FLASH;
		break;
	case m_glow:
		data[0] = M_GLOW;
		break;
	case m_stars:
		data[0] = M_STARS;
		data[2] = efg_get_stars_sporn_rate();
		data[1] = efg_get_update_interval()?(1021-efg_get_update_interval())/4:0;
		break;
	case m_segment_4:
		data[0] = M_SEGMENT_4;
		break;
	case m_segment_8:
		data[0] = M_SEGMENT_8;
		break;
	case m_segment_custom:
		data[0] = M_SEGMENT_CUSTOM;
		break;
	case m_random:
		break;
	case m_snow:
		data[0] = M_SNOW;
		data[1] = efg_get_update_interval();
		data[2] = efg_get_snow_sporn_rate();
		break;
	}
	uint8_t color = efg_get_current_color_mode();
	switch(color){
	case c_steady:
		data[5] = C_STEADY;
		data[6] = efg_get_r_value();
		data[7] = efg_get_g_value();
		data[8] = efg_get_b_value();
		break;
	case c_fade_all:
		data[5] = C_FADE_ALL;
		data[6] = efg_get_val_value();
		data[8] = efg_get_c_update_interval()?2039.368/efg_get_c_update_interval():0;
		break;
	case c_switch_all:
		data[5] = C_SWITCH_ALL;
		data[6] = efg_get_val_value();
		data[8] = efg_get_c_update_interval()?16317.736/efg_get_c_update_interval():0;
		break;
	case c_fade_3:
		data[5] = C_FADE_3;
		data[6] = efg_get_val_value();
		data[8] = efg_get_c_update_interval()?255.796/efg_get_c_update_interval():0;
		break;
	case c_switch_3:
		data[5] = C_SWITCH_3;
		data[6] = efg_get_val_value();
		data[8] = efg_get_c_update_interval()?16317.736/efg_get_c_update_interval():0;
		break;
	case c_rainbow:
		data[5] = C_RAINBOW;
		data[6] = efg_get_val_value();
		data[7] = efg_get_c_segment_width();
		break;
	case c_rainbow_chase_f:
		data[5] = C_RAINBOW_CHASE_F;
		data[6] = efg_get_val_value();
		data[7] = efg_get_c_segment_width();
		data[8] = efg_get_c_update_interval()?(765+256.0/1021)/efg_get_c_update_interval()/SPEED_FACTOR:0;
		break;
	case c_rainbow_chase_b:
		data[5] = C_RAINBOW_CHASE_B;
		data[6] = efg_get_val_value();
		data[7] = efg_get_c_segment_width();
		data[8] = efg_get_c_update_interval()?(765+256.0/1021)/efg_get_c_update_interval()/SPEED_FACTOR:0;
		break;
	case c_snow_steady:
		data[5] = C_STEADY;
		data[6] = efg_get_val_value();
		data[7] = efg_get_c_segment_width();
		break;
	case c_snow_rainbow:
		data[5] = C_RAINBOW;
		data[6] = efg_get_val_value();
		data[7] = efg_get_c_segment_width();
		break;
	}
}

uint8_t dmx_encode_get_data(uint8_t pos){
	if(pos >= NDMXVALUES){
		return 0;
	}
	return data[pos];
}
