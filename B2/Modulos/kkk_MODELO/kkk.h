#ifndef __KKK_H
#define __KKK_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_KKK 4

/*
			PERIFERICO USADO: 
			PINOUT USED:

*/

typedef struct{
	int a;
} MSGQUEUE_OBJ_KKK;

int init_Th_kkk(void);
osMessageQueueId_t get_id_MsgQueue_kkk(void);

#endif
