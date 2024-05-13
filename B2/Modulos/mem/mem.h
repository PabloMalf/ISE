#ifndef __TTF_H
#define __TTF_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef enum {WR = 0x08, RD = 0x00}cmd_t;

typedef struct{
	cmd_t cmd;
	char data [5][24];
} MSGQUEUE_OBJ_TTF_MOSI;

int init_Th_ttf(void);
osMessageQueueId_t get_id_MsgQueue_ttf_miso(void);
osMessageQueueId_t get_id_MsgQueue_ttf_mosi(void);

typedef struct{
  char adtos[50][24];
} MSGQUEUE_OBJ_TTF_MISO;



#endif
