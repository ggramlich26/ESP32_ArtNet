/*
 * dmx_decoder.h
 *
 *  Created on: Nov 12, 2017
 *      Author: rebotko
 */

#ifndef DMX_DECODER_H_
#define DMX_DECODER_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																											//
//											DMX Protocol													//
//																											//
//						General overview of a transmission in ws2812 mode									//
//		------------------------------------------------------------------									//
//		| M_CMD | M_D0 | M_D1 | M_D2 | M_D3 | C_CMD | C_D0 | C_D1 | C_D2 |									//
//		------------------------------------------------------------------									//
//			0	    1	   2	  3		4		5		6	   7	  8										//
//																											//
//		M_CMD: Motion effect command																		//
//				This can be any of the M_XXXXX definitions below											//
//		M_DX: Data command affecting the motion effect. The meaning depends on the selected motion effect.	//
//			M_CMD == M_CHASE_F			|																	//
//			M_CMD == M_CHASE_B			|																	//
//			M_CMD == M_CHASE_CROSS		|																	//
//			M_CMD == M_CHASE_MEET		|																	//
//										M_D0:	Effect speed												//
//													The speed in which the effect runs						//
//													0 pauses the effect at the current position				//
//													1= slowest, 255=fastest									//
//										M_D1:	Segment width												//
//													The number of LEDs that form one segment				//
//										M_D2:	Segment distance											//
//													Percentage of the whole strip that separates two		//
//														consecutive segments								//
//													Value: 0-255 (100 means 100% = on strip width)			//
//										M_D3:	Unused														//
//																											//
//			M_CMD == M_CHASE_BLURR_F	|																	//
//			M_CMD == M_CHASE_BLURR_FB	|																	//
//			M_CMD == M_CHASE_TEAR_F		|																	//
//			M_CMD == M_CHASE_TEAR_FB	|																	//
//										M_D0-M_D2:	Like above												//
//										M_D3:		Blur width												//
//														The number of LEDs that the blur is wide			//
//															in each used direction							//
//			M_CMD == M_FLASH			|																	//
//			M_CMD == M_GLOW				|																	//
//										M_D0:		Effect speed											//
//														as above											//
//										M_D1-M_D3:	Unused													//
//			M_CMD == M_STARS			|																	//
//										M_D0:		Effect speed											//
//														The speed with which a star vanishes again			//
//										M_D1:		Star sporn rate											//
//														The probability (in 1/255) used each 100ms 			//
//															that a star sporns until the maximum 			//
//															number of stars is reached.						//
//															0=no new stars; 255=one star per 100ms			//
//										M_D2-M_D3:	Unused													//
//			M_CMD == M_SEGMENT_4		|																	//
//			M_CMD == M_SEGMENT_8		|																	//
//										M_D0:		First segment to be activated							//
//										M_D1:		Second segment to be activated							//
//										M_D2:		Third segment to be activated							//
//										M_D3:		Unused													//
//			M_CMD == M_SEGMENT_CUSTOM	|																	//
//										M_D0:		Offset in percent of the whole strip before the			//
//														first segment starts								//
//										M_D1		Length in percent of the whole strip of the 			//
//														first segment										//
//										M_D2:		Offset in percent of the whole strip before the			//
//														second segment starts								//
//										M_D3		Length in percent of the whole strip of the 			//
//														second segment										//
//			M_CMD == M_EQUALIZER		|																	//
//			M_CMD == M_EQUALIZER_MIRRORED|																	//
//										M_D0:		Percent up to which the strip shall be activated		//
//														Values up to 255 (255=whole strip)					//
//										M_D1-M_D3:	Unused													//
//			M_CMD == M_SNOW 			|																	//
//										M_D0:		Effect speed											//
//														The time in ms divided by 4 untill the snow			//
//															flake vanishes again							//
//										M_D1:		Snow sporn rate											//
//														The probability (in 1/255) used each 100ms 			//
//															that a snow flake sporns until the maximum 		//
//															number of snow flakes is reached.				//
//															0=no new snow ; 255=one flake per 100ms			//
//										M_D2-M_D3:	Unused													//
//										For the Snow effect only color steady and rainbow are supported		//
//																											//
//																											//
//		C_CMD: Color effect command																			//
//				This can be any of the C_XXXXX definitions below											//
//		C_DX: Data command affecting the color effect. The meaning depends on the selected color effect.	//
//			C_CMD == C_STEADY			|																	//
//										C_D0:		Red														//
//										C_D1:		Green													//
//										C_D2:		Blue													//
//			C_CMD == C_FADE_ALL			|																	//
//			C_CMD == C_SWITCH_ALL		|																	//
//			C_CMD == C_FADE_3			|																	//
//			C_CMD == C_SWITCH_3			|																	//
//										C_D0:		Brightness												//
//														The brightness of each LED from 0 to 255			//
//										C_D1:		Unused													//
//										C_D2:		Color Effect Speed										//
//														The speed in which the effect runs					//
//														0 pauses the effect at the current position			//
//														1= slowest, 255=fastest								//
//			C_CMD == C_RAINBOW			|																	//
//										C_D0:		Brightness												//
//														The brightness of each LED from 0 to 255			//
//										C_D1:		Segment Width											//
//														Given in % of the whole strip. It specifies			//
//														after what distance the color pattern repeats		//
//										C_D2:		Unused													//
//			C_CMD == C_RAINBOW_CHASE_F	|																	//
//			C_CMD == C_RAINBOW_CHASE_B	|																	//
//										C_D0:		Brightness												//
//														The brightness of each LED from 0 to 255			//
//										C_D1:		Segment Width											//
//														Given in % of the whole strip. It specifies			//
//														after what distance the color pattern repeats		//
//										C_D2:		Color Effect Speed										//
//														The speed in which the effect runs					//
//														0 pauses the effect at the current position			//
//														1= slowest, 255=fastest								//
//																											//
//																											//
//						General overview of a transmission in rgb mode										//
//		----------------------																				//
//		| RED | GREEN | BLUE |																				//
//		----------------------																				//
//		   0	  1		 2																					//
//																											//
//																											//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define M_STEADY				0x00
#define	M_CHASE_F				0x01
#define	M_CHASE_B				0x02
#define M_CHASE_CROSS			0x03
#define	M_CHASE_MEET			0x04
#define	M_CHASE_BLURR_F			0x05
#define	M_CHASE_BLURR_FB		0x06
#define	M_TEAR_F				0x07
#define	M_TEAR_FB				0x08
#define	M_FLASH					0x09
#define	M_GLOW					0x0A
#define	M_STARS					0x0B
#define M_SEGMENT_4				0x0C
#define	M_SEGMENT_8				0x0D
#define	M_SEGMENT_CUSTOM		0x0E
#define	M_EQUALIZER				0x0F
#define	M_EQUALIZER_MIRRORED	0x10
#define	M_SNOW					0x11

#define C_STEADY				0x00
#define	C_FADE_ALL				0x01
#define	C_SWITCH_ALL			0x02
#define	C_FADE_3				0x03
#define C_SWITCH_3				0x04
#define	C_RAINBOW				0x05
#define	C_RAINBOW_CHASE_F		0x06
#define	C_RAINBOW_CHASE_B		0x07


enum strip_mode{ws2812,
	rgb};

void dmx_decode(uint8_t *data, uint8_t strip_mode);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* DMX_DECODER_H_ */
