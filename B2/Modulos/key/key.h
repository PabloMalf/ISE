#ifndef __KEYPAD_H
#define __KEYPAD_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"



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

#define KEY_FLAG_ON					1U << 0
#define KEY_FLAG_OFF				1U << 1

typedef struct{
	char key;
} MSGQUEUE_OBJ_KEY;

int init_Th_key(void);
osThreadId_t get_id_Th_key(void);
osMessageQueueId_t get_id_MsgQueue_key(void);

#endif
