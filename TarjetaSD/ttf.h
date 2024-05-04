#ifndef __TTF_H
#define __TTF_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef struct{
	int a; //KKK placeholder
} MSGQUEUE_OBJ_TTF;

int init_Th_ttf(void);
osMessageQueueId_t get_id_MsgQueue_ttf(void);

#endif
