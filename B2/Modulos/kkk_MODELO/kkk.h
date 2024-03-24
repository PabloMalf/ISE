#ifndef __kkk_H
#define __kkk_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_kkk 4

/*
			PERIFERICO USADO: I2C1 
			PINOUT USED:

*/

typedef struct{

} MSGQUEUE_OBJ_kkk;

int init_Th_kkk(void);
osMessageQueueId_t get_id_MsgQueue_kkk(void);

#endif
