#include "rgb.h"

#define MSGQUEUE_OBJECTS_RGB 4

static osThreadId_t id_Th_rgb;
static osMessageQueueId_t id_MsgQueue_rgb;

int init_Th_rgb(void);
static void Th_rgb(void *arg);
static int Init_MsgQueue_rgb(void);

static int Init_MsgQueue_rgb(){
	id_MsgQueue_rgb = osMessageQueueNew(MSGQUEUE_OBJECTS_RGB, sizeof(MSGQUEUE_OBJ_RGB), NULL);
	if(id_MsgQueue_rgb == NULL)
		return (-1); 
	return(0);
}


int init_Th_rgb(void){
	id_Th_rgb = osThreadNew(Th_rgb, NULL, NULL);
	if(id_Th_rgb == NULL)
		return(-1);
	return(Init_MsgQueue_rgb());
}

osMessageQueueId_t get_id_MsgQueue_rgb(void){
	return id_MsgQueue_rgb;
}

static void init_rgb(TIM_HandleTypeDef *htim){
	GPIO_InitTypeDef sgpio = {0};
	TIM_OC_InitTypeDef soc = {0};

	__HAL_RCC_GPIOE_CLK_ENABLE();
	sgpio.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_13;
	sgpio.Mode = GPIO_MODE_AF_PP;
	sgpio.Alternate = GPIO_AF1_TIM1;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &sgpio);

	__HAL_RCC_TIM1_CLK_ENABLE();
	htim->Instance = TIM1;
	htim->Init.Prescaler = 839;
	htim->Init.Period = 255;	//1khz parpadeo
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_PWM_Init(htim);

	soc.OCMode = TIM_OCMODE_PWM1;
	soc.Pulse = 0;
	soc.OCPolarity = TIM_OCPOLARITY_HIGH;
	soc.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(htim, &soc, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(htim, &soc, TIM_CHANNEL_2);
	HAL_TIM_PWM_ConfigChannel(htim, &soc, TIM_CHANNEL_3);

	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);

	HAL_TIM_Base_Start(htim);
}


static void Th_rgb(void *argument){
	MSGQUEUE_OBJ_RGB msg_rgb;
	TIM_HandleTypeDef htim;
	init_rgb(&htim);

	while(1){
		if (osOK == osMessageQueueGet(id_MsgQueue_rgb, &msg_rgb, NULL, osWaitForever)){
			__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL_1, msg_rgb.r); //r
			__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL_2, msg_rgb.g); //g
			__HAL_TIM_SET_COMPARE(&htim, TIM_CHANNEL_3, msg_rgb.b); //b
		}
	}
}

