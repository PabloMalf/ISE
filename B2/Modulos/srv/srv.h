#ifndef __SRV_H
#define __SRV_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef struct{
  char adtos[50][20];
  uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (pila)
} MSGQUEUE_OBJ_SRV;

int init_Th_srv(void);
osThreadId_t get_id_Th_srv(void);
osMessageQueueId_t get_id_MsgQueue_srv(void);

#endif

