#ifndef __TTF_H
#define __TTF_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define REGISTROS 15
#define CAMPOS 5

typedef enum {WR, RD}cmd_t;
typedef enum {REG, USER}fich_t;

typedef struct{
	fich_t fichero;
	cmd_t cmd;
	char data [50];
} MSGQUEUE_OBJ_TTF_MOSI;

int init_Th_ttf(void);
osMessageQueueId_t get_id_MsgQueue_ttf_miso(void);
osMessageQueueId_t get_id_MsgQueue_ttf_mosi(void);


typedef struct{
  char valor[20];
} string;

typedef struct{
  string datos[REGISTROS][CAMPOS];
} MSGQUEUE_OBJ_TTF_MISO;

#endif

