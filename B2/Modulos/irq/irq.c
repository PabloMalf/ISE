#include "irq.h"

#define FLAG_IRQ				1U << 2

extern uint8_t teclapulsada;
extern osThreadId_t id_Th_key;

void EXTI9_5_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(teclapulsada==0)
	osThreadFlagsSet(id_Th_key, FLAG_IRQ);
}
