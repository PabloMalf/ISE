#ifndef __LCD_H
#define __LCD_H

#include "cmsis_os2.h"

#define MSGQUEUE_OBJECTS_LCD 4

/*
			I2C2
			PINOUT USED:
			SCL	->	PB8
			SDA	->	PB9

*/

typedef enum {ON = 0, OFF = 1}back_light_t;

typedef struct{
	char L0 [20];
	char L1 [20];
	char L2 [20];
	char L3 [20];
	back_light_t back_light;
} MSGQUEUE_OBJ_LCD;

int init_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

#endif
