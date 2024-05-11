#ifndef __IRQ_H
#define __IRQ_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

void EXTI9_5_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
