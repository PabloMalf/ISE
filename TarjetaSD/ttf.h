#ifndef __TTF_H
#define __TTF_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef enum {WR = 0x08, RD = 0x00}lcd_state_t;

typedef struct{
	lcd_state_t state;
	char name [20];
} MSGQUEUE_OBJ_TTF;

int init_Th_ttf(void);
osMessageQueueId_t get_id_MsgQueue_ttf(void);

typedef struct{
	char idTarjeta[6];
	char fechaHora [19];
	char tipoAcceso ;
} entrada;

#define MAX_USU 10



#endif
