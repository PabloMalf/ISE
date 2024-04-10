#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"


#define MSGQUEUE_OBJECTS_KEYPAD 4

/* PERIFERICO USADO: GPIO
	 PINOUT USED:  
			 PD0 -> C2
			 PD1 -> R1			
			 PD3 -> R2	
			 PD4 -> R3	
			 PD5 -> C3				 
			 PD6 -> R4
			 PD7 -> C1				 
*/

#define ROW1_PIN GPIO_PIN_1
#define ROW2_PIN GPIO_PIN_3
#define ROW3_PIN GPIO_PIN_4
#define ROW4_PIN GPIO_PIN_6
#define COL1_PIN GPIO_PIN_7
#define COL2_PIN GPIO_PIN_0
#define COL3_PIN GPIO_PIN_5


typedef struct{
	char  k;
} MSGQUEUE_OBJ_KEYPAD;

int init_Th_keypad(void);
osMessageQueueId_t get_id_MsgQueue_keypad(void);

#endif
