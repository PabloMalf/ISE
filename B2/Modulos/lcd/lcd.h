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

typedef enum {ON = 0x08, OFF = 0x00}lcd_state_t;

typedef struct{
	lcd_state_t state;
	char L0 [21]; //DO NOT USE STRINGS LARGER THAN 20 CHARACTERS U NEED TO ALLOW TO FIT '\0'
	char L1 [21];
	char L2 [21];
	char L3 [21];
} MSGQUEUE_OBJ_LCD;

int init_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

#endif
