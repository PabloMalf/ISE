#include "adc.h"

#define VREF 3.3f
#define RESOLUTION_12B 4096U

void myADC_Init(ADC_HandleTypeDef *hadc){
	static GPIO_InitTypeDef sgpio = {0};
	static ADC_ChannelConfTypeDef sadc = {0};
	
	//PC0(ADC1_IN10)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	sgpio.Pin = GPIO_PIN_0;
	sgpio.Mode = GPIO_MODE_ANALOG;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOC, &sgpio);
	
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	hadc->Instance = ADC1;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV; //
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.NbrOfConversion = 1;
	hadc->Init.ScanConvMode = DISABLE;
	HAL_ADC_Init(hadc);
	
	sadc.Channel = 10;
	sadc.Rank = 1;
	sadc.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(hadc, &sadc);
}


uint32_t myADC_Get_Voltage(ADC_HandleTypeDef *hadc){
	static HAL_StatusTypeDef status;
	uint32_t voltage;
	
	HAL_ADC_Start(hadc);
	
	do{
		status = HAL_ADC_PollForConversion(hadc, 0);
	}while(status != HAL_OK);
	
	HAL_ADC_Stop(hadc);
	
	voltage = HAL_ADC_GetValue(hadc);
	
	return voltage;
}


uint32_t myADC_Get_Resolution(void){
	return RESOLUTION_12B;
}


float myADC_Get_ReferenceVoltage(void){
	return VREF;
}
