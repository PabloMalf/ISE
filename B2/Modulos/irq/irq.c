#include "irq.h"

#define FLAG_IRQ				1U << 2
#define FLAG_ACCESO     1U << 7

extern uint8_t teclapulsada;
extern osThreadId_t id_Th_key;

extern osThreadId_t id_Th_principal;

void EXTI9_5_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}


void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_0) osThreadFlagsSet(id_Th_principal, FLAG_ACCESO);
	if(!teclapulsada) osThreadFlagsSet(id_Th_key, FLAG_IRQ);
}
