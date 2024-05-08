#ifndef __BUZ_H
#define __BUZ_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

/*
			TIM3
			PINOUT USED:
			TIM3_CH2	->	PA7

*/

typedef struct{
	uint32_t frequency_hz;
	uint32_t duration_ms;
	uint8_t volume;
} MSGQUEUE_OBJ_BUZ;

int init_Th_buz(void);
osMessageQueueId_t get_id_MsgQueue_buz(void);

#endif
