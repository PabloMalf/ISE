#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "LED.h"
#include "Board_LED.h"
#include "stdio.h"
 
static osThreadId_t tid_LED;

osThreadId_t get_id_Th_LED (void)	{ return tid_LED;}

int Init_Th_LED(void){
  tid_LED = osThreadNew(tid_LED, NULL, NULL);
  if (tid_LED == NULL){
    return(-1);
  }
 
  return(0);
}


void Th_LED (void *arg){
	uint32_t flags;
	
	LED_Initialize();
	
	while(1){
		flags = osThreadFlagsWait((F_G_ON | F_G_OFF | F_G_500MS), osFlagsWaitAny, osWaitForever);
		if(flags & F_G_ON){LED_On(0x00);}
		
		
		if(flags & (F_R_OFF | F_G_OFF | F_B_OFF){}
		if(flags & F_G_500MS){
			
		}
//		if(flags & KKK){}
//		if(flags & ...){}
	}
}