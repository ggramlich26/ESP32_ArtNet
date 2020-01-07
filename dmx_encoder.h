/*
 * dmx_encoder.h
 *
 *  Created on: Jan 7, 2020
 *      Author: tsugua
 */

#ifndef DMX_ENCODER_H_
#define DMX_ENCODER_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void dmx_encode();
uint8_t dmx_encode_get_data(uint8_t pos);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* DMX_ENCODER_H_ */
