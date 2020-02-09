/*
 * effect_generator.h
 *
 *  Created on: 09.11.2017
 *      Author: Rebotko
 */

#ifndef EFFECT_GENERATOR_H_
#define EFFECT_GENERATOR_H_

#include "stdint.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS2812
#define SPEED_FACTOR		(NUMBER_LEDS*5/150./STRIP_LENGTH)

void efg_update();
void efg_init();
void efg_inc_time_diff(uint16_t diff);
uint8_t* efg_get_data();

void update_brightness(uint8_t);
void reset_brightness();
uint8_t calculate_brightness(uint16_t);

void efg_normalize_values();

void update_brightness_steady(uint8_t);
void update_brightness_chase_f(uint8_t);
void update_brightness_chase_b(uint8_t);
void update_brightness_chase_cross(uint8_t);
void update_brightness_chase_meet(uint8_t);
void update_brightness_chase_blurr_f(uint8_t);
void update_brightness_chase_blurr_fb(uint8_t);
void update_brightness_tear_f(uint8_t);
void update_brightness_tear_fb(uint8_t);
void update_brightness_flash(uint8_t);
void update_brightness_glow(uint8_t);
void update_brightness_stars(uint8_t);
void update_brightness_segment_4(uint8_t);
void update_brightness_segment_8(uint8_t);
void update_brightness_segment_custom(uint8_t);
void update_brightness_random(uint8_t);
void update_brightness_snow(uint8_t);

uint8_t calculate_brightness_steady(uint16_t);
uint8_t calculate_brightness_chase_f(uint16_t);
uint8_t calculate_brightness_chase_b(uint16_t);
uint8_t calculate_brightness_chase_cross(uint16_t);
uint8_t calculate_brightness_chase_meet(uint16_t);
uint8_t calculate_brightness_chase_blurr_f(uint16_t);
uint8_t calculate_brightness_chase_blurr_fb(uint16_t);
uint8_t calculate_brightness_tear_f(uint16_t);
uint8_t calculate_brightness_tear_fb(uint16_t);
uint8_t calculate_brightness_flash(uint16_t);
uint8_t calculate_brightness_glow(uint16_t);
uint8_t calculate_brightness_stars(uint16_t);
uint8_t calculate_brightness_segment_4(uint16_t);
uint8_t calculate_brightness_segment_8(uint16_t);
uint8_t calculate_brightness_segment_custom(uint16_t);
uint8_t calculate_brightness_random(uint16_t);
uint8_t calculate_brightness_snow(uint16_t);

void reset_brightness_steady();
void reset_brightness_chase_f();
void reset_brightness_chase_b();
void reset_brightness_chase_cross();
void reset_brightness_chase_meet();
void reset_brightness_chase_blurr_f();
void reset_brightness_chase_blurr_fb();
void reset_brightness_tear_f();
void reset_brightness_tear_fb();
void reset_brightness_flash();
void reset_brightness_glow();
void reset_brightness_stars();
void reset_brightness_segment_4();
void reset_brightness_segment_8();
void reset_brightness_segment_custom();
void reset_brightness_random();
void reset_brightness_snow();

void calculate_color(uint8_t*, uint16_t, uint8_t);
void update_color(uint8_t);
void reset_color();
void write_color(uint8_t*, uint8_t, uint8_t, uint8_t);

void update_color_steady(uint8_t);
void update_color_fade_all(uint8_t);
void update_color_switch_all(uint8_t);
void update_color_fade_3(uint8_t);
void update_color_switch_3(uint8_t);
void update_color_rainbow(uint8_t);
void update_color_rainbow_chase_f(uint8_t);
void update_color_rainbow_chase_b(uint8_t);
void update_color_snow_steady(uint8_t);
void update_color_snow_rainbow(uint8_t);

void reset_color_steady();
void reset_color_fade_all();
void reset_color_switch_all();
void reset_color_fade_3();
void reset_color_switch_3();
void reset_color_rainbow();
void reset_color_rainbow_chase_f();
void reset_color_rainbow_chase_b();
void reset_color_snow_steady();
void reset_color_snow_rainbow();

void calculate_color_steady(uint8_t*, uint16_t, uint8_t);
void calculate_color_fade_all(uint8_t*, uint16_t, uint8_t);
void calculate_color_switch_all(uint8_t*, uint16_t, uint8_t);
void calculate_color_fade_3(uint8_t*, uint16_t, uint8_t);
void calculate_color_switch_3(uint8_t*, uint16_t, uint8_t);
void calculate_color_rainbow(uint8_t*, uint16_t, uint8_t);
void calculate_color_rainbow_chase_f(uint8_t*, uint16_t, uint8_t);
void calculate_color_rainbow_chase_b(uint8_t*, uint16_t, uint8_t);
void calculate_color_snow_steady(uint8_t*, uint16_t, uint8_t);
void calculate_color_snow_rainbow(uint8_t*, uint16_t, uint8_t);

enum motion_effects {m_steady,
                    m_chase_f,
                    m_chase_b,
                    m_chase_cross,
                    m_chase_meet,
                    m_chase_blurr_f,
                    m_chase_blurr_fb,
                    m_tear_f,
                    m_tear_fb,
                    m_flash,
                    m_glow,
                    m_stars,
                    m_segment_4,
                    m_segment_8,
                    m_segment_custom,
                    m_random,
					m_snow
};

enum color_effects {
                    c_steady,
                    c_fade_all,
                    c_switch_all,
                    c_fade_3,
                    c_switch_3,
                    c_rainbow,
                    c_rainbow_chase_f,
                    c_rainbow_chase_b,
					c_snow_steady,
					c_snow_rainbow
};

void efg_set_current_motion(uint8_t);
void efg_set_segment_width(uint8_t);
void efg_set_segment_distance(uint8_t);
void efg_set_update_interval(uint16_t);
void efg_set_blurr_width(uint8_t);
void efg_set_segment_1(uint8_t);
void efg_set_segment_2(uint8_t);
void efg_set_segment_3(uint8_t);
void efg_set_segment_offset(uint8_t);
void efg_set_stars_sporn_rate(uint8_t);

void efg_color_set_mode(uint8_t);
void efg_color_set_val_value(uint8_t);
void efg_color_set_segment_width(uint8_t);
void efg_color_set_update_interval(uint16_t);

void efg_set_steady();
void efg_set_chase(uint8_t mode, uint8_t segment_width, uint8_t segment_distance,
		uint16_t update_interval);
void efg_set_blurr(uint8_t mode, uint8_t segment_width, uint8_t segment_distance,
		uint16_t update_interval, uint8_t blurr_width);
void efg_set_flash(uint8_t mode, uint16_t update_interval);
void efg_set_stars(uint16_t update_interval, uint8_t sporn_rate);
void efg_set_segment(uint8_t mode, uint8_t segment_1, uint8_t segment_2, uint8_t segment_3, uint8_t segment_4);
void efg_set_segment_custom(uint8_t segment1_offset, uint8_t segment1_width,
		uint8_t segment2_offset, uint8_t segment2_width);
void efg_set_snow(uint16_t update_interval, uint8_t sporn_rate);

void efg_color_set_steady(uint8_t r, uint8_t g, uint8_t b);
void efg_color_set_fade_switch(uint8_t mode, uint16_t update_interval, uint8_t val);
void efg_color_set_rainbow_chase(uint8_t mode, uint16_t update_interval, uint8_t val,
		uint8_t segment_width);
void efg_color_set_rainbow(uint8_t val, uint8_t segment_width);
void efg_color_set_snow_steady(uint8_t r, uint8_t g, uint8_t b);
void efg_color_set_snow_rainbow(uint8_t val, uint8_t segment_width);

uint8_t efg_get_red();
uint8_t efg_get_green();
uint8_t efg_get_blue();

uint8_t efg_get_current_motion();
uint8_t efg_get_segment_width();
uint8_t efg_get_segment_dist();
uint16_t efg_get_update_interval();
uint8_t efg_get_blur_width();
uint8_t efg_get_stars_sporn_rate();
uint8_t efg_get_snow_sporn_rate();
uint8_t efg_get_current_color_mode();
uint8_t efg_get_r_value();
uint8_t efg_get_g_value();
uint8_t efg_get_b_value();
uint8_t efg_get_val_value();
uint16_t efg_get_c_update_interval();
uint8_t efg_get_c_segment_width();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* EFFECT_GENERATOR_H_ */
