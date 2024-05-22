#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef struct{
  char adtos[50][20];
  uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (pila)
} MSGQUEUE_OBJ_SRV;

typedef struct{
  char nombre[15];
  char pin[4];
  char id[20];
  char sexo;
} registro;

typedef struct {
  registro registros[10];
} MSGQUEUE_OBJ_REG;


int init_Th_srv(void);
osThreadId_t get_id_Th_srv(void);
osMessageQueueId_t get_id_MsgQueue_srv(void);
osMessageQueueId_t get_id_MsgQueue_reg(void);
#endif

