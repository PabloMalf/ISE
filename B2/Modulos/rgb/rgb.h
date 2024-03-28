#ifndef __RGB_H
#define __RGB_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_RGB 4

/*
			TIM1
			PINOUT USED:
			TIM1_CH1	->	PE9
			TIM1_CH2	->	PE11
			TIM1_CH3	->	PE13
*/

typedef struct{ 
	uint8_t r;
	uint8_t g;
	uint8_t b;
} MSGQUEUE_OBJ_RGB;

int init_Th_rgb(void);
osMessageQueueId_t get_id_MsgQueue_rgb(void);

#endif
