#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "parpadeo.h"
#include "stdio.h"
 
osThreadId_t tid_Thread_Parpadeo;
 
void Parpadeo (void *argument);
 
int Init_Parpadeo(void){
  tid_Thread_Parpadeo = osThreadNew(Parpadeo, NULL, NULL);
  if (tid_Thread_Parpadeo == NULL){
    return(-1);
  }
 
  return(0);
}
 
void Parpadeo (void *argument) {
  while(1){
		osThreadFlagsWait(1, osFlagsWaitAny, osWaitForever);
		// Parpadea durante 5 segundos
		printf("Parpadea durante 5 segundos\n");
		for(int i = 0; i < 20; i++){		// Parpadea 20 veces
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			osDelay(250);									// 0,25 ms * 20 =  5 segundos
		}
		osThreadYield();
  }
}
