#include "key.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

#define MSGQUEUE_OBJECTS_KEY 4

#define NUM_ROW 4
#define NUM_COL 3

#define FLAG_IRQ				1U << 2
#define FLAG_REBOTES		1U << 3
#define FLAG_MUESTREO		1U << 4
#define FLAG_CHECK_KEY	1U << 5


const char char_teclas [NUM_ROW] [NUM_COL] = {{'1', '2', '3'},
																							{'4', '5', '6'},
																							{'7', '8', '9'},
																							{'*', '0', '#'}};

const uint32_t PIN_ROW [NUM_ROW] = {GPIO_PIN_1, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_6};
const uint32_t PIN_COL [NUM_COL] = {GPIO_PIN_7, GPIO_PIN_0, GPIO_PIN_5};

static osTimerId_t tim_id_Rebotes;
static osTimerId_t tim_id_Muestreo;
static osTimerId_t tim_id_KeyOn;

static osThreadId_t id_Th_key;
static void Th_key(void *arg);

static osMessageQueueId_t id_MsgQueue_key;
static int Init_MsgQueue_key(void);

static void	GPIO_Init(void);
static void Timer_Callback_Rebotes(void);
static void Timer_Callback_Muestreo(void);
static void Timer_Callback_KeyOn(void);

static uint8_t col_on = 0;
static uint8_t teclapulsada = 0;


osMessageQueueId_t get_id_MsgQueue_key(void){
	return id_MsgQueue_key;
}


osThreadId_t get_id_Th_key(void){
	return id_Th_key;
}


int init_Th_key(void){
	id_Th_key = osThreadNew(Th_key, NULL, NULL);
	if(id_Th_key == NULL)
		return(-1);
	return(0);
}


static int Init_MsgQueue_key(){
	id_MsgQueue_key = osMessageQueueNew(MSGQUEUE_OBJECTS_KEY, sizeof(MSGQUEUE_OBJ_KEY), NULL);
	if(id_MsgQueue_key == NULL)
		return (-1); 
	return(0);
}


static void GPIO_Init(void){
	static GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_0 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6;	 
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP ;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


void EXTI9_5_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(teclapulsada==0)
	osThreadFlagsSet(id_Th_key, FLAG_IRQ);
}


static void Init_Tims (void){                   
  uint32_t exec = 1U;
  tim_id_Rebotes = osTimerNew((osTimerFunc_t)&Timer_Callback_Rebotes, osTimerOnce, &exec, NULL);
	tim_id_Muestreo = osTimerNew((osTimerFunc_t)&Timer_Callback_Muestreo, osTimerPeriodic, &exec, NULL);
	tim_id_KeyOn = osTimerNew((osTimerFunc_t)&Timer_Callback_KeyOn, osTimerPeriodic, &exec, NULL);
}


static void Timer_Callback_Rebotes(void){
	osThreadFlagsSet(id_Th_key, FLAG_REBOTES);
}


static void Timer_Callback_Muestreo(void){
	osThreadFlagsSet(id_Th_key, FLAG_MUESTREO);
}


static void Timer_Callback_KeyOn(void){
	osThreadFlagsSet(id_Th_key, FLAG_CHECK_KEY);
}

static void Th_key(void *argument){
	MSGQUEUE_OBJ_KEY msg_key;
	uint32_t flags;
	uint8_t	row = 0;
	uint8_t col = 0;
	
	Init_MsgQueue_key();
	GPIO_Init();
	Init_Tims();
	
	while(1){
		flags = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
		if(flags & KEY_FLAG_ON){
			osTimerStart(tim_id_Muestreo, 10U);
		}
		
		
		if(flags & FLAG_IRQ){
			osTimerStart(tim_id_Rebotes, 50U);
		}
		
		
		if(flags & FLAG_REBOTES){
			osTimerStop(tim_id_Muestreo);
			HAL_GPIO_WritePin(GPIOD, PIN_ROW[0]| PIN_ROW[1] | PIN_ROW[2] | PIN_ROW[3],GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, PIN_ROW[row], GPIO_PIN_RESET);
			for(col = 0; col < NUM_COL; col++){
				if(!HAL_GPIO_ReadPin(GPIOD, PIN_COL[col])){
						if(teclapulsada == 0){ 
							osTimerStart(tim_id_KeyOn,75);
							col_on=col;
							teclapulsada = 1;
							msg_key.key = char_teclas[row][col];
							osMessageQueuePut(id_MsgQueue_key, &msg_key, 0, 0);
						}
				}
			}
			osTimerStart(tim_id_Muestreo, 10U);
		}
		
		
		if(flags & FLAG_MUESTREO){
			row += (row < NUM_ROW-1) ? 1 : -row;
			HAL_GPIO_WritePin(GPIOD, PIN_ROW[row], GPIO_PIN_RESET);
		}
		
		
		if(flags & FLAG_CHECK_KEY){
			if(HAL_GPIO_ReadPin(GPIOD, PIN_COL[col_on])==1){
				teclapulsada=0;
				osTimerStop(tim_id_KeyOn);
			}
		}
	}
}


