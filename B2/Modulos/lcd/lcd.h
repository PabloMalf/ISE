#ifndef __LCD_H
#define __LCD_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_LCD 4

/*
			I2C1 
			PINOUT USED:
			PB8	->	SCL
			PB9	->	SDA
*/

typedef struct{
	char L0 [20];
	char L1 [20];
	char L2 [20];
	char L3 [20];
} MSGQUEUE_OBJ_LCD;

int init_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

#endif
