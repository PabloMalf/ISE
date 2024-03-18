#include "stm32f4xx_hal.h"
#include "Board_LED.h"


typedef struct _GPIO_PIN {
	GPIO_TypeDef *port;
	uint16_t			pin;
	uint16_t			reserved;
} GPIO_PIN;


static const GPIO_PIN LED_PIN[] = {
	{ GPIOB, GPIO_PIN_0, 0U },
	{ GPIOB, GPIO_PIN_7, 0U },
	{ GPIOB, GPIO_PIN_14, 0U }
};

#define LED_COUNT (sizeof(LED_PIN)/sizeof(GPIO_PIN))


int32_t LED_Initialize (void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin	 = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull	= GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	return 0;
}


int32_t LED_Uninitialize (void) {
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14);
	return 0;
}


int32_t LED_On (uint32_t num) {
	int32_t retCode = 0;

	if (num < LED_COUNT) {
		HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_SET);
	}
	else {
		retCode = 1;
	}

	return retCode;
}


int32_t LED_Off (uint32_t num) {
	int32_t retCode = 0;

	if (num < LED_COUNT) {
		HAL_GPIO_WritePin(LED_PIN[num].port, LED_PIN[num].pin, GPIO_PIN_RESET);
	}
	else {
		retCode = 1;
	}

	return retCode;
}


int32_t LED_SetOut (uint32_t val) {
	uint32_t n;
	for(n = 0; n < LED_COUNT; n++) {
		if(val & (1 << n))
			LED_On (n);
		else	
			LED_Off(n);
	}
	return 0;
}


uint32_t LED_GetCount (void) {
	return LED_COUNT;
}
