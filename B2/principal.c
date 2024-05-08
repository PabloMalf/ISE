#include "principal.h"
#include "stm32f4xx_hal.h"

//kkk change ip Net_Conifg_ETH_0
//kkk change buzz pin 7 

typedef enum {BATTERY_PSU, MAIN_PSU} ali_state_t;

typedef struct{
	MSGQUEUE_OBJ_BUZ buz;
	//MSGQUEUE_OBJ_CAM cam;
	MSGQUEUE_OBJ_KEY key;
	MSGQUEUE_OBJ_LCD lcd;
	MSGQUEUE_OBJ_NFC nfc;
	MSGQUEUE_OBJ_RGB rgb;
	MSGQUEUE_OBJ_TTF ttf;
	mytime_t time;
}msg_t;

typedef struct{
	ali_state_t ali_state;
	msg_t msg;
}data_t;

extern mytime_t g_time;
static osThreadId_t id_Th_principal;

int init_Th_principal(void);
static void Th_principal(void *arg);


int init_Th_principal(void){
	id_Th_principal = osThreadNew(Th_principal, NULL, NULL);
	if(id_Th_principal == NULL)
		return(-1);
	return(	init_Th_buz() |
					//init_Th_cam() |
					init_Th_key() |
					init_Th_lcd() |
					init_Th_nfc() |
					init_Th_rgb() |
					init_Th_rtc() |
					//init_Th_srv() |
					init_Th_ttf());
}

static void init_pin_ali(void){
	GPIO_InitTypeDef sgpio = {0};
	
	//PIN_ALI_STATE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_PULLUP;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &sgpio);
	
	//PIN_ALI_STATE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_PULLUP;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &sgpio);
}


static void enable_mov(void){
	static GPIO_InitTypeDef  GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


static int time_updated(data_t* data){
	if(data->msg.time.seg == g_time.seg){
		data->msg.time = g_time;
		return 1;
	}
	return 0;
}


static void mode_main_psu(data_t* data){
	//init SV
	enable_mov();
	
	while(1){
		if(time_updated(data)){
			//
		}
	}
}

static void mode_battery_psu(data_t* data){
	
}

static void Th_principal(void *argument){
	data_t data;
	init_pin_ali();
	
	data.ali_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) ? BATTERY_PSU : MAIN_PSU;
	switch(data.ali_state){
		case MAIN_PSU:
			mode_main_psu(&data);
		break;
		
		case BATTERY_PSU:
			mode_battery_psu(&data);
		break;
	}
}
