/*
 * efect_generator.c
 *
 *  Created on: Nov 9, 2017
 *      Author: tsugua
 */

#include <stdint.h>

#include "effect_generator.h"



uint8_t current_motion;	//current motion effect
uint8_t segment_width;		//number of LEDs, that form one segment in a chase for example
uint8_t	segment_dist;		//distance between segments in percent of the whole strip length
uint16_t update_interval;	//time in ms until the pattern moves one step
uint8_t m_blurr_width;      //number of LEDs in each direction that the blurr is wide
uint8_t segment_1;          //segment to be turned on, 0=nothing, or percentage of whole strip forming one segment to be turned on
                            //      in case of m_segment_custom
uint8_t segment_2;          //segment to be turned on, 0=nothing
uint8_t segment_3;          //segment to be turned on, 0=nothing
uint8_t segment_4;          //segment to be turned on, 0=nothing
uint8_t segment_offset_1;   //offset before starting segment 1
uint8_t segment_offset_2;   //offset before starting segment 2
uint8_t stars_sporn_rate;   //probability that a star sporns in each update cycle
uint8_t snow_sporn_rate;   //probability that a star sporns in each update cycle



uint8_t current_color_mode;	//current color mode
uint8_t r_value;			//red intensity
uint8_t g_value;			//green intensity
uint8_t b_value;			//blue intensity
uint8_t val_value;          //value value
uint16_t c_update_interval; //time in ms until the color configuration moves on one step
uint8_t c_segment_width;    //width in percent of the whole strip until the color configuration repeats


uint8_t data[3*NUMBER_LEDS];
uint8_t time_passed = 0;	//the time passed since the last update occurred

void efg_init(){
    current_motion = m_steady;
    segment_width = 1;
    segment_dist = 100;
    update_interval = 20;
    m_blurr_width = 2;
    current_color_mode = c_steady;
    r_value = 0;
    g_value = 0;
    b_value = 0;
    val_value = 255;
    c_update_interval = 100;
    c_segment_width = 100;
    segment_1 = 12;
    segment_2 = 3;
    segment_3 = 1;
    segment_4 = 0;
    segment_offset_1 = 0;
    segment_offset_2 = 0;
    stars_sporn_rate = 22;
    snow_sporn_rate = 22;
}

void efg_inc_time_diff(uint16_t diff){
    time_passed += diff;
}

uint8_t* efg_get_data(){
    return data;
}

void efg_update(){
//	cli();
    uint8_t local_time_passed = time_passed;
    time_passed = 0;
//	sei();
    update_brightness(local_time_passed);
    update_color(local_time_passed);
    for (uint16_t i = 0; i < NUMBER_LEDS; i++){
        uint8_t brightness = calculate_brightness(i);
        calculate_color(&data[3*i], i, brightness);
    }
}

/**
 * This function normalizes all values to the length of the used LED strip and to it's number of LEDs
 */
void efg_normalize_values(){
	if(current_motion == m_chase_f ||
			current_motion == m_chase_b ||
			current_motion == m_chase_cross ||
			current_motion == m_chase_meet ||
			current_motion == m_chase_blurr_f ||
			current_motion == m_chase_blurr_fb ||
			current_motion == m_tear_f ||
			current_motion == m_tear_fb
			){
		if(update_interval > 0){
			update_interval = (uint32_t)update_interval/SPEED_FACTOR;
			if(update_interval == 0){
				update_interval = 1;
			}
		}
		if(segment_width > 0){
			segment_width = (uint32_t)segment_width*SPEED_FACTOR;
			if(segment_width == 0){
				segment_width = 1;
			}
		}
		if(m_blurr_width > 0){
			m_blurr_width = (uint32_t)m_blurr_width*SPEED_FACTOR;
			if(m_blurr_width == 0){
				m_blurr_width = 1;
			}
		}
	}
	if(current_color_mode == c_rainbow_chase_f ||
			current_color_mode == c_rainbow_chase_b){
		if(c_update_interval > 0){
			c_update_interval = (uint32_t)c_update_interval/SPEED_FACTOR;
			if(c_update_interval == 0){
				c_update_interval = 1;
			}
		}
	}
}

void efg_set_current_motion(uint8_t motion){
	if(current_motion == motion){
		return;
	}
	reset_brightness();
	current_motion = motion;
	}

void efg_set_segment_width(uint8_t width){
	segment_width = width;
}

void efg_set_segment_distance(uint8_t dist){
	segment_dist = dist;
}

void efg_set_update_interval(uint16_t interval){
	update_interval = interval;
}

void efg_set_blurr_width(uint8_t width){
	m_blurr_width = width;
}

void efg_set_segment_1(uint8_t segment){
	segment_1 = segment;
}

void efg_set_segment_2(uint8_t segment){
	segment_2 = segment;
}

void efg_set_segment_3(uint8_t segment){
	segment_3 = segment;
}

void efg_set_segment_4(uint8_t segment){
	segment_4 = segment;
}

void efg_set_segment_offset_1(uint8_t offset){
	segment_offset_1 = offset;
}

void efg_set_segment_offset_2(uint8_t offset){
	segment_offset_2 = offset;
}

void efg_set_stars_sporn_rate(uint8_t rate){
	stars_sporn_rate = rate;
}

void efg_set_snow_sporn_rate(uint8_t rate){
	snow_sporn_rate = rate;
}

//void efg_set_current_color_mode(uint8_t mode){
//	if(current_color_mode == mode){
//		return;
//	}
//	reset_color();
//	current_color_mode = mode;
//}
//
//void efg_set_r_value(uint8_t r){
//	r_value = r;
//}
//
//void efg_set_g_value(uint8_t g){
//	g_value = g;
//}
//
//void efg_set_b_value(uint8_t b){
//	b_value = b;
//}
//
//void efg_set_val_value(uint8_t val){
//	val_value = val;
//}
//
//void efg_set_c_update_interval(uint16_t interval){
//	c_update_interval = interval;
//}
//
//void efg_set_c_segment_width(uint8_t width){
//	c_segment_width = width;
//}

void efg_set_steady(){
	efg_set_current_motion(m_steady);
}

void efg_set_chase(uint8_t mode, uint8_t segment_width, uint8_t segment_distance,
		uint16_t update_interval){
	efg_set_segment_width(segment_width);
	efg_set_segment_distance(segment_distance);
	efg_set_update_interval(update_interval);
	efg_set_current_motion(mode);
}

void efg_set_blurr(uint8_t mode, uint8_t segment_width, uint8_t segment_distance,
		uint16_t update_interval, uint8_t blurr_width){
	efg_set_segment_width(segment_width);
	efg_set_segment_distance(segment_distance);
	efg_set_update_interval(update_interval);
	efg_set_blurr_width(blurr_width);
	efg_set_current_motion(mode);
}

void efg_set_flash(uint8_t mode, uint16_t update_interval){
	efg_set_update_interval(update_interval);
	efg_set_current_motion(mode);
}

void efg_set_stars(uint16_t update_interval, uint8_t sporn_rate){
	efg_set_stars_sporn_rate(sporn_rate);
	efg_set_update_interval(update_interval);
	efg_set_current_motion(m_stars);
}

void efg_set_snow(uint16_t update_interval, uint8_t sporn_rate){
	efg_set_snow_sporn_rate(sporn_rate);
	efg_set_update_interval(update_interval);
	efg_set_current_motion(m_snow);
}

void efg_set_segment(uint8_t mode, uint8_t segment_1, uint8_t segment_2, uint8_t segment_3, uint8_t segment_4){
 efg_set_segment_1(segment_1);
 efg_set_segment_2(segment_2);
 efg_set_segment_3(segment_3);
 efg_set_segment_4(segment_4);
 efg_set_current_motion(mode);
}

void efg_set_segment_custom(uint8_t segment1_offset, uint8_t segment1_width, uint8_t segment2_offset, uint8_t segment2_width){
	efg_set_segment_1(segment1_width);
	efg_set_segment_offset_1(segment1_offset);
	efg_set_segment_2(segment2_width);
	efg_set_segment_offset_2(segment2_offset);
	efg_set_current_motion(m_segment_custom);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											Brightness Functions											//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness(uint8_t time_diff){
    switch(current_motion) {
    case m_steady:
        update_brightness_steady(time_diff);
        break;
    case m_chase_f:
        update_brightness_chase_f(time_diff);
        break;
    case m_chase_b:
        update_brightness_chase_b(time_diff);
        break;
    case m_chase_cross:
        update_brightness_chase_cross(time_diff);
        break;
    case m_chase_meet:
        update_brightness_chase_meet(time_diff);
        break;
    case m_chase_blurr_f:
        update_brightness_chase_blurr_f(time_diff);
        break;
    case m_chase_blurr_fb:
        update_brightness_chase_blurr_fb(time_diff);
        break;
    case m_tear_f:
        update_brightness_tear_f(time_diff);
        break;
    case m_tear_fb:
        update_brightness_tear_fb(time_diff);
        break;
    case m_flash:
        update_brightness_flash(time_diff);
        break;
    case m_glow:
        update_brightness_glow(time_diff);
        break;
    case m_stars:
        update_brightness_stars(time_diff);
        break;
    case m_segment_4:
        update_brightness_segment_4(time_diff);
        break;
    case m_segment_8:
        update_brightness_segment_8(time_diff);
        break;
    case m_segment_custom:
        update_brightness_segment_custom(time_diff);
        break;
    case m_random:
        update_brightness_random(time_diff);
        break;
    case m_snow:
        update_brightness_snow(time_diff);
        break;
    }
}

void reset_brightness(){
    reset_brightness_steady();
    reset_brightness_chase_f();
    reset_brightness_chase_b();
    reset_brightness_chase_cross();
    reset_brightness_chase_meet();
    reset_brightness_chase_blurr_f();
    reset_brightness_chase_blurr_fb();
    reset_brightness_tear_f();
    reset_brightness_tear_fb();
    reset_brightness_flash();
    reset_brightness_glow();
    reset_brightness_stars();
    reset_brightness_segment_4();
    reset_brightness_segment_8();
    reset_brightness_segment_custom();
    reset_brightness_random();
    reset_brightness_snow();
}

uint8_t calculate_brightness(uint16_t i){
    switch(current_motion) {
    case m_steady:
        return calculate_brightness_steady(i);
        break;
    case m_chase_f:
        return calculate_brightness_chase_f(i);
        break;
    case m_chase_b:
        return calculate_brightness_chase_b(i);
        break;
    case m_chase_cross:
        return calculate_brightness_chase_cross(i);
        break;
    case m_chase_meet:
        return calculate_brightness_chase_meet(i);
        break;
    case m_chase_blurr_f:
        return calculate_brightness_chase_blurr_f(i);
        break;
    case m_chase_blurr_fb:
        return calculate_brightness_chase_blurr_fb(i);
        break;
    case m_tear_f:
        return calculate_brightness_tear_f(i);
        break;
    case m_tear_fb:
        return calculate_brightness_tear_fb(i);
        break;
    case m_flash:
        return calculate_brightness_flash(i);
        break;
    case m_glow:
        return calculate_brightness_glow(i);
        break;
    case m_stars:
        return calculate_brightness_stars(i);
        break;
    case m_segment_4:
        return calculate_brightness_segment_4(i);
        break;
    case m_segment_8:
        return calculate_brightness_segment_8(i);
        break;
    case m_segment_custom:
        return calculate_brightness_segment_custom(i);
        break;
    case m_random:
        return calculate_brightness_random(i);
        break;
    case m_snow:
        return calculate_brightness_snow(i);
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Chase Forward												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t cf_time_passed = 0;
uint16_t cf_position = 0;

void update_brightness_chase_f(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cf_time_passed += time_diff;
    cf_position += cf_time_passed / update_interval;
    cf_time_passed = cf_time_passed % update_interval;
    if(cf_position >= NUMBER_LEDS + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100){
        cf_position = NUMBER_LEDS + (cf_position - NUMBER_LEDS)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100);
    }
}

void reset_brightness_chase_f(){
    cf_time_passed = 0;
    cf_position = 0;
}

uint8_t calculate_brightness_chase_f(uint16_t i){
    if(i > cf_position){
        return 0;
    }
    if((cf_position-i)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100) < segment_width){
        return 255;
    }
    else{
        return 0;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Chase Backward												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t cb_time_passed = 0;
int16_t cb_position = NUMBER_LEDS;

void update_brightness_chase_b(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cb_time_passed += time_diff;
    cb_position -= cb_time_passed / update_interval;
    cb_time_passed = cb_time_passed % update_interval;
    if(cb_position < 0){
        if(-cb_position >= segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100){
            cb_position = -((-cb_position)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100));
        }
    }
}

void reset_brightness_chase_b(){
    cb_position = NUMBER_LEDS;
    cb_time_passed = 0;
}

uint8_t calculate_brightness_chase_b(uint16_t i){
	int16_t is = (int16_t)i;
    if(is < cb_position){
        return 0;
    }
    if((is-cb_position)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100) < segment_width){
        return 255;
    }
    else{
        return 0;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Chase Cross													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t cc_position_f = 0;
int16_t cc_position_b = NUMBER_LEDS;
uint16_t cc_time_passed = 0;

void update_brightness_chase_cross(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cc_time_passed += time_diff;
    cc_position_f += cc_time_passed / update_interval;
    cc_position_b -= cc_time_passed / update_interval;
    cc_time_passed = cc_time_passed % update_interval;

    //forward running part
    if(cc_position_f >= NUMBER_LEDS + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100){
        cc_position_f = NUMBER_LEDS + (cc_position_f - NUMBER_LEDS)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100);
    }

    //backward running part
    if(cc_position_b < 0){
        if(-cc_position_b >= segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100){
            cc_position_b = -((-cc_position_b)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100));
        }
    }
}

void reset_brightness_chase_cross(){
    cc_position_f = 0;
    cc_position_b = NUMBER_LEDS;
    cc_time_passed = 0;
}

uint8_t calculate_brightness_chase_cross(uint16_t i){
    //forward running part
    if(i <= cc_position_f){
        if((cc_position_f-i)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100) < segment_width){
            return 255;
        }
    }
    //backwards running part
    int16_t is = (int16_t)i;
    if(is >= cc_position_b){
        if((is-cc_position_b)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100) < segment_width){
            return 255;
        }
    }

    //default:
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Chase Meet													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t cm_position_f = 0;
int16_t cm_position_b = NUMBER_LEDS;
uint16_t cm_time_passed = 0;

void update_brightness_chase_meet(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cm_time_passed += time_diff;
    cm_position_f += cm_time_passed / update_interval;
    cm_position_b -= cm_time_passed / update_interval;
    cm_time_passed = cm_time_passed % update_interval;

    //forward running part
    if(cm_position_f >= NUMBER_LEDS/2 + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/200){
        cm_position_f = NUMBER_LEDS/2 + (cm_position_f - NUMBER_LEDS/2)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/200);
    }

    //backward running part
    if(cm_position_b <= NUMBER_LEDS/2){
        if(NUMBER_LEDS/2-cm_position_b >= segment_width + (uint32_t)segment_dist*NUMBER_LEDS/200){
            cm_position_b = NUMBER_LEDS/2-((NUMBER_LEDS/2-cm_position_b)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/200));
        }
    }
}

void reset_brightness_chase_meet(){
    cm_position_f = 0;
    cm_position_b = NUMBER_LEDS;
}

uint8_t calculate_brightness_chase_meet(uint16_t i){
    //forward running part
    if(i <= cm_position_f && i <= NUMBER_LEDS/2){
        if((cm_position_f-i)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/200) < segment_width){
            return 255;
        }
    }
    //backwards running part
    if((int32_t)i >= cm_position_b && i >= NUMBER_LEDS/2){
        if((i-cm_position_b)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/200) < segment_width){
            return 255;
        }
    }

    //default:
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Chase Blurr Forward 										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//uint16_t cbf_time_passed = 0;
//uint16_t cbf_position = 0;
//
//void update_brightness_chase_blurr_f(uint8_t time_diff){
//    if(update_interval == 0){
//        return;
//    }
//    cbf_time_passed += time_diff;
//    cbf_position += cbf_time_passed / update_interval;
//    cbf_time_passed = cbf_time_passed % update_interval;
//    if(cbf_position >= NUMBER_LEDS + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100 + m_blurr_width){
//        cbf_position = NUMBER_LEDS + (cbf_position - NUMBER_LEDS)%(segment_width+(uint32_t)segment_dist*NUMBER_LEDS/100);
//    }
//}

float cbf_position = 0;

void update_brightness_chase_blurr_f(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cbf_position += (float)time_diff / update_interval;
    while(cbf_position >= NUMBER_LEDS + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100 + m_blurr_width +1){
    	cbf_position -= segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100;
    }
}

void reset_brightness_chase_blurr_f(){
    cbf_position = 0;
}

uint8_t calculate_brightness_chase_blurr_f(uint16_t i){
    if(i - m_blurr_width - 1 > cbf_position){
        return 0;
    }
    while(i + m_blurr_width + segment_width < cbf_position){
    	i += segment_width + (uint32_t) segment_dist*NUMBER_LEDS/100;
    }
    if(i > cbf_position + m_blurr_width + 1){
        return 0;
    }
    //right blurr
    if(i > cbf_position){
    	return 255-(uint8_t)(255*(i-cbf_position)/(m_blurr_width+1));
    }
    if(i + segment_width - 1 >= cbf_position){
    	return 255;
    }
    //left blurr
    else{
    	return 255-(uint8_t)(255*(cbf_position-(i+segment_width-1))/(m_blurr_width+1));
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//										Chase Blurr Forward Backward 										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

float cbfb_position = 0;
uint16_t cbfb_mode = 0; //0 = forward; 1 = backward

void update_brightness_chase_blurr_fb(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    if(cbfb_mode == 0){
		cbfb_position += (float)time_diff / update_interval;
        if(cbfb_position > NUMBER_LEDS){
            cbfb_mode = 1;
            cbfb_position = 2*NUMBER_LEDS - cbfb_position;
        }
    }
    else{
		cbfb_position -= (float)time_diff / update_interval;
        if(cbfb_position < 0){
            cbfb_mode = 0;
            cbfb_position = -cbfb_position;
        }
    }
}

void reset_brightness_chase_blurr_fb(){
    cbfb_position = 0;
}

uint8_t calculate_brightness_chase_blurr_fb(uint16_t i){
    if(i - m_blurr_width - 1 > cbfb_position){
        return 0;
    }
    while(i + m_blurr_width + segment_width < cbfb_position){
    	i += segment_width + (uint32_t) segment_dist*NUMBER_LEDS/100;
    }
    if(i > cbfb_position + m_blurr_width + 1){
        return 0;
    }
    //right blurr
    if(i > cbfb_position){
    	return 255-(uint8_t)(255*(i-cbfb_position)/(m_blurr_width+1));
    }
    if(i + segment_width - 1 >= cbfb_position){
    	return 255;
    }
    //left blurr
    else{
    	return 255-(uint8_t)(255*(cbfb_position-(i+segment_width-1))/(m_blurr_width+1));
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//                                                  Tear Forward     										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t tf_time_passed = 0;
float tf_position = 0;

void update_brightness_tear_f(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    tf_position += (float)time_diff / update_interval;
    while(tf_position >= NUMBER_LEDS + segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100 + m_blurr_width +1){
    	tf_position -= segment_width + (uint32_t)segment_dist*NUMBER_LEDS/100;
    }
}

void reset_brightness_tear_f(){
    tf_position = 0;
}

uint8_t calculate_brightness_tear_f(uint16_t i){
    if(i - 1 > tf_position){
        return 0;
    }
    while(i + m_blurr_width + segment_width < tf_position){
    	i += segment_width + (uint32_t) segment_dist*NUMBER_LEDS/100;
    }
    if(i > tf_position + 1){
        return 0;
    }
    //fade in
    if(i > tf_position){
    	return 255-(uint8_t)(255*(i-tf_position));
    }
    if(i + segment_width - 1 >= tf_position){
    	return 255;
    }
    //left blurr
    else{
    	return 255-(uint8_t)(255*(tf_position-(i+segment_width-1))/(m_blurr_width+1));
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//                                            Tear Forward Backward    										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

float tfb_position = 0;
uint8_t tfb_mode = 0; //0=forward; 1=backwawrd

void update_brightness_tear_fb(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    if(tfb_mode == 0){
		tfb_position += (float)time_diff / update_interval;
        if(tfb_position > NUMBER_LEDS + segment_width + m_blurr_width + 1){
            tfb_mode = 1;
            tfb_position = 2*NUMBER_LEDS + segment_width + m_blurr_width + 2 - tfb_position;
        }
    }
    else{
		tfb_position -= (float)time_diff / update_interval;
        if(tfb_position < -segment_width - m_blurr_width - 1){
            tfb_mode = 0;
            tfb_position = -tfb_position - segment_width - m_blurr_width - 2;
        }
    }
}

void reset_brightness_tear_fb(){
    tfb_position = 0;
    tfb_mode = 0;
}

uint8_t calculate_brightness_tear_fb(uint16_t i){
    if(i - m_blurr_width - 1 > tfb_position){
        return 0;
    }
    while(i + m_blurr_width + segment_width < tfb_position){
    	i += segment_width + (uint32_t) segment_dist*NUMBER_LEDS/100;
    }
    if(i > tfb_position + m_blurr_width + 1){
        return 0;
    }
    //right blurr
    if(i > tfb_position){
    	if(tfb_mode == 1){
			return 255-(uint8_t)(255*(i-tfb_position)/(m_blurr_width+1));
    	}
    	else if(i - 1 < tfb_position){
    		return 255 - (uint8_t)((i-tfb_position)*255);
    	}
    	else{
    		return 0;
    	}
    }
    if(i + segment_width - 1 >= tfb_position){
    	return 255;
    }
    //left blurr
    else{
    	if(tfb_mode == 0){
			return 255-(uint8_t)(255*(tfb_position-(i+segment_width-1))/(m_blurr_width+1));
    	}
    	else if(i + segment_width > tfb_position){
    		return 255 - (uint8_t)((tfb_position-i-segment_width+1)*255);
    	}
    	else{
    		return 0;
    	}
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Flash													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int16_t f_time_to_change = 0;
uint8_t f_state = 0;

void update_brightness_flash(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    f_time_to_change -= time_diff;
    if(f_time_to_change <= 0){
        f_state = !f_state;
        f_time_to_change += update_interval;
    }
}

void reset_brightness_flash(){
    f_time_to_change = 0;
    f_state = 0;
}

uint8_t calculate_brightness_flash(uint16_t i){
    if(f_state){
        return 255;
    }
    else{
        return 0;
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Glow													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t g_intensity = 255;
uint8_t g_direction = 0;		//0 = decreasing, >0 = increasing
uint16_t g_time_passed = 0;

void update_brightness_glow(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    g_time_passed += time_diff;
    uint8_t diff = g_time_passed*4/update_interval;
    g_time_passed = ((g_time_passed*4) % update_interval)/4;
    if(g_direction){
        if(255-g_intensity < diff){
            g_direction = 0;
            g_intensity = 255-(diff-(255-g_intensity));
        }
        else{
            g_intensity += diff;
        }
    }
    else{
        if(g_intensity < diff){
            g_direction = 1;
            g_intensity = diff-g_intensity;
        }
        else{
            g_intensity -= diff;
        }
    }

}

void reset_brightness_glow(){
    g_intensity = 255;
    g_direction = 0;
}

inline uint8_t calculate_brightness_glow(uint16_t i){
    return g_intensity;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Stars													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUMBER_STARS    10
#define STAR_SPORN_DIFF 100 //time in ms between two possible sporns of stars

uint8_t stars_brightness[NUMBER_STARS];
uint16_t stars_position[NUMBER_STARS];

uint16_t stars_time_passed = 0; //time passed since the last update
uint8_t stars_sporn_time = 0;   //time passed since the last possibility of a star sporn

uint8_t get_rand(){
    static uint16_t lfsr = 0xACE1u;         /* Any nonzero start state will work. */
    uint16_t bit;                    /* Must be 16bit to allow bit<<15 later in the code */

    for(uint8_t i = 0; i < 8; i++)
    {
        /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
        bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
        lfsr =  (lfsr >> 1) | (bit << 15);
    }

    return lfsr;

}

void update_brightness_stars(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    stars_time_passed += time_diff;
    uint8_t diff = (stars_time_passed/update_interval)*4;
    stars_sporn_time += time_diff;
    stars_time_passed = stars_time_passed % update_interval;
    uint8_t sporn = 0;
    if(stars_sporn_time >= STAR_SPORN_DIFF){
        stars_sporn_time = stars_sporn_time % STAR_SPORN_DIFF;
        sporn = get_rand() < stars_sporn_rate;
    }

    //update all stars and eventually let a new one sporn
    if(diff){
        for(uint8_t i = 0; i < NUMBER_STARS; i++){
            if(stars_brightness[i]){
                if(stars_brightness[i] < diff){
                    if(sporn){
                        stars_brightness[i] = 255;
                        stars_position[i] = (((uint32_t)get_rand())<<8|get_rand())*NUMBER_LEDS/0xFFFF;
                        sporn = 0;
                    }
                    else{
                        stars_brightness[i] = 0;
                    }
                }
                else{
                    stars_brightness[i] -= diff;
                }
            }
            else if(sporn){
                stars_brightness[i] = 255;
                stars_position[i] = (((uint32_t)get_rand())<<8|get_rand())*NUMBER_LEDS/0xFFFF;
                sporn = 0;
            }
        }
    }
}

void reset_brightness_stars(){
    stars_time_passed = 0;
    for(uint8_t i = 0; i < NUMBER_STARS; i++){
        stars_brightness[i] = 0;
    }
}

uint8_t calculate_brightness_stars(uint16_t i){
    for(uint8_t j = 0; j < NUMBER_STARS; j++){
        if(i == stars_position[j]){
            return stars_brightness[j];
        }
    }
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Steady													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness_steady(uint8_t time_diff){}

void reset_brightness_steady(){}

uint8_t calculate_brightness_steady(uint16_t i){
    return 255;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Segment 4           										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness_segment_4(uint8_t time_diff){}

void reset_brightness_segment_4(){}

uint8_t calculate_brightness_segment_4(uint16_t i){
    if(i<NUMBER_LEDS/4){
        if(segment_1==1||segment_2==1||segment_3==1||segment_4==1){
            return 255;
        }
    }
    else if(i < NUMBER_LEDS/2){
        if(segment_1==2||segment_2==2||segment_3==2||segment_4==2){
            return 255;
        }
    }
    else if(i < (uint16_t)NUMBER_LEDS*3/4){
        if(segment_1==3||segment_2==3||segment_3==3||segment_4==3){
            return 255;
        }
    }
    else{
        if(segment_1==4||segment_2==4||segment_3==4||segment_4==4){
            return 255;
        }
    }
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Segment 8           										//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness_segment_8(uint8_t time_diff){}

void reset_brightness_segment_8(){}

uint8_t calculate_brightness_segment_8(uint16_t i){
    if(i<NUMBER_LEDS/8){
        if(segment_1==1||segment_2==1||segment_3==1||segment_4==1){
            return 255;
        }
    }
    else if(i < NUMBER_LEDS/4){
        if(segment_1==2||segment_2==2||segment_3==2||segment_4==2){
            return 255;
        }
    }
    else if(i < (uint16_t)NUMBER_LEDS*3/8){
        if(segment_1==3||segment_2==3||segment_3==3||segment_4==3){
            return 255;
        }
    }
    else if(i < NUMBER_LEDS/2){
        if(segment_1==4||segment_2==4||segment_3==4||segment_4==4){
            return 255;
        }
    }
    else if(i < (uint16_t)NUMBER_LEDS*5/8){
        if(segment_1==5||segment_2==5||segment_3==5||segment_4==5){
            return 255;
        }
    }
    else if(i < (uint16_t)NUMBER_LEDS*3/4){
        if(segment_1==6||segment_2==6||segment_3==6||segment_4==6){
            return 255;
        }
    }
    else if(i < (uint16_t)NUMBER_LEDS*7/8){
        if(segment_1==7||segment_2==7||segment_3==7||segment_4==7){
            return 255;
        }
    }
    else{
        if(segment_1==8||segment_2==8||segment_3==8||segment_4==8){
            return 255;
        }
    }
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Segment Custom												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness_segment_custom(uint8_t time_diff){}

void reset_brightness_segment_custom(){}

inline uint8_t calculate_brightness_segment_custom(uint16_t i){
    if(i >= (uint16_t)NUMBER_LEDS*segment_offset_1/100 &&
            i < NUMBER_LEDS*((uint16_t)segment_offset_1+segment_1)/100){
        return 255;
    }
    else if(i >= (uint16_t)NUMBER_LEDS*segment_offset_2/100 &&
            i < NUMBER_LEDS*((uint16_t)segment_offset_2+segment_2)/100){
        return 255;
    }
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Random													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_brightness_random(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
}

void reset_brightness_random(){

}

uint8_t calculate_brightness_random(uint16_t i){
    return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Snow													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NUMBER_SNOW		10
#define SNOW_SPORN_DIFF 100 //time in ms between two possible sporns of snow flakes

uint8_t snow_brightness[NUMBER_STARS];
uint16_t snow_position[NUMBER_STARS];

uint16_t snow_time_passed = 0; //time passed since the last update
uint8_t snow_sporn_time = 0;   //time passed since the last possibility of a star sporn

void update_brightness_snow(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    snow_time_passed += time_diff;
    uint8_t diff = (uint16_t)snow_time_passed*255/update_interval;
    snow_sporn_time += time_diff;
    snow_time_passed = ((uint16_t)snow_time_passed*255 % update_interval)/255;
    uint8_t sporn = 0;
    if(snow_sporn_time >= SNOW_SPORN_DIFF){
        snow_sporn_time = snow_sporn_time % SNOW_SPORN_DIFF;
        sporn = get_rand() < snow_sporn_rate;
    }

    //update all snow flakes and eventually let a new one sporn
    if(diff){
        for(uint8_t i = 0; i < NUMBER_STARS; i++){
            if(snow_brightness[i]){
                if(snow_brightness[i] < diff){
                    if(sporn){
                        snow_brightness[i] = 255;
                        snow_position[i] = (((uint32_t)get_rand())<<8|get_rand())*NUMBER_LEDS/0xFFFF;
                        sporn = 0;
                    }
                    else{
                        snow_brightness[i] = 0;
                    }
                }
                else{
                    snow_brightness[i] -= diff;
                }
            }
            else if(sporn){
                snow_brightness[i] = 255;
                snow_position[i] = (((uint32_t)get_rand())<<8|get_rand())*NUMBER_LEDS/0xFFFF;
                sporn = 0;
            }
        }
    }
}

void reset_brightness_snow(){
    snow_time_passed = 0;
    for(uint8_t i = 0; i < NUMBER_STARS; i++){
        snow_brightness[i] = 0;
    }
}

#define SNOW_LOW_LEVEL	254
uint8_t calculate_brightness_snow(uint16_t i){
    for(uint8_t j = 0; j < NUMBER_STARS; j++){
        if(i == snow_position[j] && snow_brightness[j] > 0){
            return 255;
        }
    }
    return SNOW_LOW_LEVEL;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Color Functions												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_color(uint8_t time_passed){
    switch(current_color_mode) {
    case c_steady:
        update_color_steady(time_passed);
        break;
    case c_fade_all:
        update_color_fade_all(time_passed);
        break;
    case c_switch_all:
        update_color_switch_all(time_passed);
        break;
    case c_fade_3:
        update_color_fade_3(time_passed);
        break;
    case c_switch_3:
        update_color_switch_3(time_passed);
        break;
    case c_rainbow:
        update_color_rainbow(time_passed);
        break;
    case c_rainbow_chase_f:
        update_color_rainbow_chase_f(time_passed);
        break;
    case c_rainbow_chase_b:
        update_color_rainbow_chase_b(time_passed);
        break;
    case c_snow_steady:
        update_color_snow_steady(time_passed);
        break;
    case c_snow_rainbow:
        update_color_snow_rainbow(time_passed);
        break;
    }
}

void reset_color(){
    reset_color_steady();
    reset_color_fade_all();
    reset_color_switch_all();
    reset_color_fade_3();
    reset_color_switch_3();
    reset_color_rainbow();
    reset_color_rainbow_chase_f();
    reset_color_rainbow_chase_b();
    reset_color_snow_steady();
    reset_color_snow_rainbow();
}

void calculate_color(uint8_t* address, uint16_t i, uint8_t brightness){
    switch(current_color_mode) {
    case c_steady:
        calculate_color_steady(address, i, brightness);
        break;
    case c_fade_all:
        calculate_color_fade_all(address, i, brightness);
        break;
    case c_switch_all:
        calculate_color_switch_all(address, i, brightness);
        break;
    case c_fade_3:
        calculate_color_fade_3(address, i, brightness);
        break;
    case c_switch_3:
        calculate_color_switch_3(address, i, brightness);
        break;
    case c_rainbow:
        calculate_color_rainbow(address, i, brightness);
        break;
    case c_rainbow_chase_f:
        calculate_color_rainbow_chase_f(address, i, brightness);
        break;
    case c_rainbow_chase_b:
        calculate_color_rainbow_chase_b(address, i, brightness);
        break;
    case c_snow_steady:
    	calculate_color_snow_steady(address, i, brightness);
    	break;
    case c_snow_rainbow:
    	calculate_color_snow_rainbow(address, i, brightness);
    	break;
    }
}

void efg_color_set_mode(uint8_t mode){
	if(mode == current_color_mode){
		return;
	}
	reset_color();
	current_color_mode = mode;
}

void efg_color_set_val_value(uint8_t val){
	val_value = val;
}

void efg_color_set_update_interval(uint16_t interval){
	c_update_interval = interval;
}

void efg_color_set_segment_width(uint8_t width){
	c_segment_width = width;
}

void efg_color_set_steady(uint8_t r, uint8_t g, uint8_t b){
	r_value = r;
	g_value = g;
	b_value = b;
	efg_color_set_mode(c_steady);
}

void efg_color_set_fade_switch(uint8_t mode, uint16_t update_interval, uint8_t val){
	efg_color_set_update_interval(update_interval);
	efg_color_set_val_value(val);
	efg_color_set_mode(mode);
}

void efg_color_set_rainbow_chase(uint8_t mode, uint16_t update_interval, uint8_t val, uint8_t segment_width){
	efg_color_set_update_interval(update_interval);
	efg_color_set_val_value(val);
	efg_color_set_segment_width(segment_width);
	efg_color_set_mode(mode);
}

void efg_color_set_rainbow(uint8_t val, uint8_t segment_width){
	efg_color_set_val_value(val);
	efg_color_set_segment_width(segment_width);
	efg_color_set_mode(c_rainbow);
}

void efg_color_set_snow_steady(uint8_t r, uint8_t g, uint8_t b){
	r_value = r;
	g_value = g;
	b_value = b;
	efg_color_set_mode(c_snow_steady);
}

void efg_color_set_snow_rainbow(uint8_t val, uint8_t segment_width){
	efg_color_set_val_value(val);
	efg_color_set_segment_width(segment_width);
	efg_color_set_mode(c_snow_rainbow);
}

inline uint8_t efg_get_red(){
	return r_value;
}

inline uint8_t efg_get_green(){
	return g_value;
}

inline uint8_t efg_get_blue(){
	return b_value;
}




inline void write_color(uint8_t* address, uint8_t r, uint8_t g, uint8_t b){
#ifdef WS2811
    *(address++) = b;
    *(address++) = r;
    *address = g;
#elif defined WS2812
    *(address++) = g;
    *(address++) = r;
    *address = b;
#else
    *(address++) = g;
    *(address++) = r;
    *address = b;
#endif
}

//typedef struct {
//    double r;       // a fraction between 0 and 1
//    double g;       // a fraction between 0 and 1
//    double b;       // a fraction between 0 and 1
//} rgb_double;

//typedef struct {
//    double h;       // angle in degrees
//    double s;       // a fraction between 0 and 1
//    double v;       // a fraction between 0 and 1
//} hsv_double;

typedef struct {
    uint8_t r;       // a fraction between 0 and 1
    uint8_t g;       // a fraction between 0 and 1
    uint8_t b;       // a fraction between 0 and 1
} rgb;

//typedef struct {
//    uint8_t h;       // angle in degrees
//    uint8_t s;       // a fraction between 0 and 1
//    uint8_t v;       // a fraction between 0 and 1
//} hsv;

//static inline hsv hsv_double2hsv_8(hsv_double in){
//    hsv out;
//    out.h = (uint8_t)(in.h*255/360);
//    out.s = (uint8_t)(in.s*255);
//    out.v = (uint8_t)(in.v*255);
//    return out;
//}

//static inline rgb rgb_double2rgb_8(rgb_double in){
//    rgb out;
//    out.r = in.r*255;
//    out.g = in.g*255;
//    out.b = in.b*255;
//    return out;
//}

//static hsv rgb2hsv(rgb in_8)
//{
//    hsv_double         out;
//    rgb_double in;
//    in.r = in_8.r/255.0;
//    in.g = in_8.g/255.0;
//    in.b = in_8.b/255.0;

//    double      min, max, delta;

//    min = in.r < in.g ? in.r : in.g;
//    min = min  < in.b ? min  : in.b;

//    max = in.r > in.g ? in.r : in.g;
//    max = max  > in.b ? max  : in.b;

//    out.v = max;                                // v
//    delta = max - min;
//    if (delta < 0.00001)
//    {
//        out.s = 0;
//        out.h = 0; // undefined, maybe nan?
//        return hsv_double2hsv_8(out);
//    }
//    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
//        out.s = (delta / max);                  // s
//    } else {
//        // if max is 0, then r = g = b = 0
//        // s = 0, v is undefined
//        out.s = 0.0;
//        out.h = 0; //usualy schould be NAN here                           // its now undefined
//        return hsv_double2hsv_8(out);
//    }
//    if( in.r >= max )                           // > is bogus, just keeps compilor happy
//        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
//    else
//    if( in.g >= max )
//        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
//    else
//        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

//    out.h *= 60.0;                              // degrees

//    if( out.h < 0.0 )
//        out.h += 360.0;

//    return hsv_double2hsv_8(out);
//}


//static rgb hsv2rgb(hsv in_8)
//{
//    double      hh, p, q, t, ff;
//    long        i;
//    rgb_double  out;
//    hsv_double	in;
//    in.h = in_8.h*360.0/255.0;
//    in.s = in_8.s/255.0;
//    in.v = in_8.v/255.0;

//    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
//        out.r = in.v;
//        out.g = in.v;
//        out.b = in.v;
//        return rgb_double2rgb_8(out);
//    }
//    hh = in.h;
//    if(hh >= 360.0) hh = 0.0;
//    hh /= 60.0;
//    i = (long)hh;
//    ff = hh - i;
//    p = in.v * (1.0 - in.s);
//    q = in.v * (1.0 - (in.s * ff));
//    t = in.v * (1.0 - (in.s * (1.0 - ff)));

//    switch(i) {
//    case 0:
//        out.r = in.v;
//        out.g = t;
//        out.b = p;
//        break;
//    case 1:
//        out.r = q;
//        out.g = in.v;
//        out.b = p;
//        break;
//    case 2:
//        out.r = p;
//        out.g = in.v;
//        out.b = t;
//        break;

//    case 3:
//        out.r = p;
//        out.g = q;
//        out.b = in.v;
//        break;
//    case 4:
//        out.r = t;
//        out.g = p;
//        out.b = in.v;
//        break;
//    case 5:
//    default:
//        out.r = in.v;
//        out.g = p;
//        out.b = q;
//        break;
//    }
//    return rgb_double2rgb_8(out);
//}

rgb rgb_table_rainbow[] = {{255,0,0}, {255,179,0}, {255,255,0}, {180,255,0}, /*{0,255,0},*/ {0,255,255}, {0,160,255}, {40,0,255}, {255,0,255}, {255,0,0}, {255,0,150}, {255,0,0}};
void compute_rgb_rainbow(uint8_t * rp, uint8_t * gp, uint8_t * bp, uint8_t t) {
  // t in the range 0..255 (for convenience)
  int segment = t>>5; // 0..7
//  uint8_t segment = t/25;
  int delta = t&31;
//  uint8_t delta = t%25;
  uint8_t a=rgb_table_rainbow[segment].r, b=rgb_table_rainbow[segment+1].r;
  *rp = a + ((delta*(b-a))>>5);
  a=rgb_table_rainbow[segment].g; b=rgb_table_rainbow[segment+1].g;
  *gp = a + ((delta*(b-a))>>5);
  a=rgb_table_rainbow[segment].b; b=rgb_table_rainbow[segment+1].b;
  *bp = a + ((delta*(b-a))>>5);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Steady													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_color_steady(uint8_t time_diff){}

void reset_color_steady(){}

void calculate_color_steady(uint8_t* address, uint16_t i, uint8_t brightness){
    write_color(address, (uint8_t)((uint16_t)brightness*r_value/255), \
            (uint8_t)((uint16_t)brightness*g_value/255), \
            (uint8_t)((uint16_t)brightness*b_value/255));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Fade All												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t cfa_color = 0;
uint16_t cfa_time_passed = 0;

void update_color_fade_all(uint8_t time_diff){
    if(c_update_interval == 0){
        return;
    }
    cfa_time_passed += time_diff;
    uint8_t diff = cfa_time_passed*4/c_update_interval;
    cfa_time_passed = ((cfa_time_passed*4)%c_update_interval)/4;
    cfa_color += diff;
}

void reset_color_fade_all(){
    cfa_time_passed = 0;
    cfa_color = 0;
}

void calculate_color_fade_all(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    compute_rgb_rainbow(&r, &g, &b, cfa_color);
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Switch All												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t csa_time_passed = 0;
uint8_t csa_index = 0;

void update_color_switch_all(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    csa_time_passed += time_diff;
    csa_index = (csa_index + csa_time_passed/c_update_interval)%8;
    csa_time_passed = csa_time_passed % c_update_interval;
}

void reset_color_switch_all(){
    csa_time_passed = 0;
    csa_index = 0;
}

void calculate_color_switch_all(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    r = rgb_table_rainbow[csa_index].r;
    g = rgb_table_rainbow[csa_index].g;
    b = rgb_table_rainbow[csa_index].b;
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Fade 3													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t cf3_index = 0;  //0=red fading in; 1=red fading out; 2=green fading in; 3=green fading out;
                        //4=blue fading in; 5=blue fading out
uint8_t cf3_value = 0;
uint16_t cf3_time_passed = 0;

void update_color_fade_3(uint8_t time_diff){
    if(c_update_interval == 0){
        return;
    }
    cf3_time_passed += time_diff;
    uint8_t diff = (cf3_time_passed / c_update_interval)*4;
    cf3_time_passed = cf3_time_passed % c_update_interval;
    if(cf3_index == 0 || cf3_index == 2 || cf3_index == 4){
        if(diff > 255-cf3_value){
            cf3_value = 255-(diff-(255-cf3_value));
            cf3_index++;
        }
        else{
            cf3_value += diff;
        }
    }
    else{
        if(diff > cf3_value){
            cf3_value = diff-cf3_value;
            cf3_index = (cf3_index+1) % 6;
        }
        else{
            cf3_value -= diff;
        }
    }
}

void reset_color_fade_3(){
    cf3_index = 0;
    cf3_value = 0;
    cf3_time_passed = 0;
}

void calculate_color_fade_3(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    r = g = b = 0;
    switch (cf3_index) {
    case 0:
    case 1:
        r = cf3_value;
        break;
    case 2:
    case 3:
        g = cf3_value;
        break;
    case 4:
    case 5:
        b = cf3_value;
        break;
    default:
        cf3_index = 0;
        break;
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Switch 3												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t cs3_index = 0;
uint16_t cs3_time_passed = 0;

void update_color_switch_3(uint8_t time_diff){
    if(update_interval == 0){
        return;
    }
    cs3_time_passed += time_diff;
    cs3_index = (cs3_index + cs3_time_passed/c_update_interval)%3;
    cs3_time_passed = cs3_time_passed % c_update_interval;
}

void reset_color_switch_3(){
    cs3_index = 0;
    cs3_time_passed = 0;
}

void calculate_color_switch_3(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    r = g = b = 0;
    switch (cs3_index) {
    case 0:
        r = 255;
        break;
    case 1:
        g = 255;
        break;
    case 2:
        b = 255;
        break;
    default:
        cs3_index = 0;
        break;
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//													Rainbow													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_color_rainbow(uint8_t time_diff){
}

void reset_color_rainbow(){

}

void calculate_color_rainbow(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    if(c_segment_width == 0){
        r = 255;
        b = g = 0;
    }
    else{
    	uint8_t divider = NUMBER_LEDS*c_segment_width/100;
    	if(divider){
			compute_rgb_rainbow(&r, &g, &b, i*255/divider);
    	}
    	else{
    		r = 255;
    		b = g = 0;
    	}
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											Rainbow Chase Forward											//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t crcf_color_offset = 0;
float crcf_time_passed = 0;

void update_color_rainbow_chase_f(uint8_t time_diff){
    if(c_update_interval == 0){
        return;
    }
    crcf_time_passed += time_diff;
    if(c_segment_width != 0){
    	uint8_t divider = NUMBER_LEDS*c_segment_width/100;
    	if(divider){
			uint8_t diff = crcf_time_passed/c_update_interval*255/divider;
			crcf_color_offset -= diff;
			crcf_time_passed -= (float)diff*c_update_interval/255*(NUMBER_LEDS*c_segment_width/100);
    	}
    	else{
    		return;
    	}
    }
}

void reset_color_rainbow_chase_f(){
    crcf_color_offset = 0;
    crcf_time_passed = 0;
}

void calculate_color_rainbow_chase_f(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    if(c_segment_width == 0){
        r = 255;
        b = g = 0;
    }
    else{
    	uint8_t divider = NUMBER_LEDS*c_segment_width/100;
    	if(divider){
			compute_rgb_rainbow(&r, &g, &b, i*255/divider + crcf_color_offset);
    	}
    	else{
    		r = 255;
    		b = g = 0;
    	}
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											Rainbow Chase Backward											//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t crcb_color_offset = 0;
float crcb_time_passed = 0;

void update_color_rainbow_chase_b(uint8_t time_diff){
    if(c_update_interval == 0){
        return;
    }
    crcb_time_passed += time_diff;
    if(c_segment_width != 0){
    	uint8_t divider = NUMBER_LEDS*c_segment_width/100;
    	if(divider){
			uint8_t diff = crcb_time_passed/c_update_interval*255/divider;
			crcb_color_offset += diff;
			crcb_time_passed -= (float)diff*c_update_interval/255*divider;
    	}
    	else{
    		return;
    	}
    }
}

void reset_color_rainbow_chase_b(){
    crcb_color_offset = 0;
    crcb_time_passed = 0;
}

void calculate_color_rainbow_chase_b(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    if(c_segment_width == 0){
        r = 255;
        b = g = 0;
    }
    else{
    	uint8_t divider = NUMBER_LEDS*c_segment_width/100;
    	if(divider){
			compute_rgb_rainbow(&r, &g, &b, i*255/divider + crcb_color_offset);
    	}
    	else{
    		r = 255;
    		b = g = 0;
    	}
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    write_color(address, r, g, b);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Snow Steady													//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_color_snow_steady(uint8_t time_diff){}

void reset_color_snow_steady(){}

void calculate_color_snow_steady(uint8_t* address, uint16_t i, uint8_t brightness){
	if(brightness != 255){
		write_color(address, (uint8_t)((uint16_t)brightness*r_value/255), \
				(uint8_t)((uint16_t)brightness*g_value/255), \
				(uint8_t)((uint16_t)brightness*b_value/255));
	}
	else{
		write_color(address, 255, 255, 255);
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												Snow Rainbow												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update_color_snow_rainbow(uint8_t time_diff){
}

void reset_color_snow_rainbow(){

}

void calculate_color_snow_rainbow(uint8_t* address, uint16_t i, uint8_t brightness){
    uint8_t r, g, b;
    if(c_segment_width == 0){
        r = 255;
        b = g = 0;
    }
    else{
        compute_rgb_rainbow(&r, &g, &b, i*255/(NUMBER_LEDS*c_segment_width/100));
    }
    uint8_t factor = (uint16_t)brightness*val_value/255;
    r = (uint16_t)r*factor/255;
    g = (uint16_t)g*factor/255;
    b = (uint16_t)b*factor/255;
    if(brightness != 255){
		write_color(address, r, g, b);
    }
    else{
    	write_color(address, val_value, val_value, val_value);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//												get functions												//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t efg_get_current_motion(){
	return current_motion;
}
uint8_t efg_get_segment_width(){
	return segment_width;
}
uint8_t efg_get_segment_dist(){
	return segment_dist;
}
uint16_t efg_get_update_interval(){
	return update_interval;
}
uint8_t efg_get_blur_width(){
	return m_blurr_width;
}
uint8_t efg_get_stars_sporn_rate(){
	return stars_sporn_rate;
}
uint8_t efg_get_snow_sporn_rate(){
	return snow_sporn_rate;
}
uint8_t efg_get_current_color_mode(){
	return current_color_mode;
}
uint8_t efg_get_r_value(){
	return r_value;
}
uint8_t efg_get_g_value(){
	return g_value;
}
uint8_t efg_get_b_value(){
	return b_value;
}
uint8_t efg_get_val_value(){
	return val_value;
}
uint16_t efg_get_c_update_interval(){
	return c_update_interval;
}
uint8_t efg_get_c_segment_width(){
	return c_segment_width;
}
