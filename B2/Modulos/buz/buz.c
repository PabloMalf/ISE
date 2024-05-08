#include "buz.h"

#define MSGQUEUE_OBJECTS_BUZ 4

static osThreadId_t id_Th_buz;
static osMessageQueueId_t id_MsgQueue_buz;

int init_Th_buz(void);
static void Th_buz(void *arg);
static int Init_MsgQueue_buz(void);

static int Init_MsgQueue_buz(){
	id_MsgQueue_buz = osMessageQueueNew(MSGQUEUE_OBJECTS_BUZ, sizeof(MSGQUEUE_OBJ_BUZ), NULL);
	if(id_MsgQueue_buz == NULL)
		return (-1); 
	return(0);
}

int init_Th_buz(void){
	id_Th_buz = osThreadNew(Th_buz, NULL, NULL);
	if(id_Th_buz == NULL)
		return(-1);
	return(Init_MsgQueue_buz());
}

osMessageQueueId_t get_id_MsgQueue_buz(void){
	return id_MsgQueue_buz;
}

static void init_buz(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *hsoc){
  static GPIO_InitTypeDef sgpio = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	sgpio.Pin= GPIO_PIN_7;
	sgpio.Mode= GPIO_MODE_AF_PP;
  sgpio.Alternate= GPIO_AF2_TIM3;
	sgpio.Pull= GPIO_NOPULL;
  sgpio.Speed= GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &sgpio);
	
	__HAL_RCC_TIM3_CLK_ENABLE();
	htim->Instance= TIM3;
	htim->Init.Prescaler = 840; // 84MHz/840 = 100KHz -> 10us
	htim->Init.Period = 100000 - 1; // 100KHz/100000 = 1Hz technically no sound
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(htim);
	
	hsoc->OCMode = TIM_OCMODE_PWM1;
	hsoc->OCPolarity = TIM_OCPOLARITY_HIGH;
	hsoc->OCFastMode = TIM_OCFAST_DISABLE;
	hsoc->Pulse = 0; // 0% (volume)
	HAL_TIM_PWM_ConfigChannel(htim, hsoc, TIM_CHANNEL_2);
}

//Una funcion que cambie la frecuencia del sonido del buzzer
static void set_tone(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *hsoc, uint32_t freq, uint8_t volume){
	htim->Init.Period = 100000 / freq - 1;
	hsoc->Pulse = ((htim->Init.Period) * (volume > 10 ? 10 : volume)) / 100;
	HAL_TIM_PWM_Init(htim);
	HAL_TIM_PWM_ConfigChannel(htim, hsoc, TIM_CHANNEL_2);
}

static void Th_buz(void *argument){
	static TIM_HandleTypeDef  htim3 = {0};
	static TIM_OC_InitTypeDef hsoc3 = {0};
	MSGQUEUE_OBJ_BUZ msg_buz;
	init_buz(&htim3, &hsoc3);
	
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_buz, &msg_buz, NULL, osWaitForever)){
			set_tone(&htim3, &hsoc3, msg_buz.frequency_hz, msg_buz.volume);
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
			osDelay(msg_buz.duration_ms);
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
		}
	}
}

