#ifndef __SRV_H
#define __SRV_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

#define MAX_USU 20
#define MSGQUEUE_OBJECTS_SRV 1

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

extern char identificacion[MAX_USU][20];
extern char fechaHora[MAX_USU][20];
extern char tipoAcceso[MAX_USU][20];
extern char nombre[MAX_USU][20];
extern char mensajeInfo[50];


static int Init_MsgQueue_srv(void);
#endif

