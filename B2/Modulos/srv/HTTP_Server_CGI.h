#ifndef __HTTPSERVERCGI_H
#define __HTTPSERVERCGI_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define REGISTROS 15
#define CAMPOS_REG 5
#define CAMPOS_USU 4

typedef struct{
  char nombre[15];
  char pin[4];
  char id[20];
  char sexo;
} registro;

typedef struct {
  registro registros[10];
} MSGQUEUE_OBJ_REG;

typedef struct{
  char valor[20];
} string;

typedef struct{
  string datos[REGISTROS][CAMPOS_REG];
	uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (pila)
} MSGQUEUE_OBJ_SRV;

int init_Th_srv(void);
osThreadId_t get_id_Th_srv(void);
osMessageQueueId_t get_id_MsgQueue_srv(void);
osMessageQueueId_t get_id_MsgQueue_reg(void);
#endif

