#ifndef __VOL_H
#define __VOL_H

#include "stm32f4xx_hal.h"

void myADC_Init(ADC_HandleTypeDef *hadc);

uint32_t	myADC_Get_Voltage(ADC_HandleTypeDef *hadc);
uint32_t	myADC_Get_Resolution(void);
float 		myADC_Get_VoltageReference(void);
#endif
