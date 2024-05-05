#include "keypad.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

#define Pulsacion 0x00000001U

// Thread Keypad
static osThreadId_t id_Th_keypad;
static void Th_keypad(void *arg);
int init_Th_keypad(void);

// Queue Keypad
static osMessageQueueId_t id_MsgQueue_keypad;
static int Init_MsgQueue_keypad(void);

static void	GPIO_Init(void);

// Timer Rebotes
osTimerId_t tim_id_Rebotes;                           
static uint32_t exec_Rebotes; 

/// Timer row
osTimerId_t tim_id_row;                           
static uint32_t exec_row;                          

uint8_t row=0;

static int Init_MsgQueue_keypad(){
	id_MsgQueue_keypad = osMessageQueueNew(MSGQUEUE_OBJECTS_KEYPAD, sizeof(MSGQUEUE_OBJ_KEYPAD), NULL);
	if(id_MsgQueue_keypad == NULL)
		return (-1); 
	return(0);
}


int init_Th_keypad(void){
	id_Th_keypad = osThreadNew(Th_keypad, NULL, NULL);
	if(id_Th_keypad == NULL)
		return(-1);
	return(Init_MsgQueue_keypad());
}


osMessageQueueId_t get_id_MsgQueue_keypad(void){
	return id_MsgQueue_keypad;
}

void GPIO_Init(){
	static GPIO_InitTypeDef  GPIO_InitStruct;
	 //Configuracion de pines
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
	/*Configure GPIO pin
			 PD0 -> C2
			 PD1 -> R1			
			 PD3 -> R2	
			 PD4 -> R3	
			 PD5 -> C3				 
			 PD6 -> R4
			 PD7 -> C1*/

  GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN |
	                      ROW3_PIN | ROW4_PIN;	 
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP ;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
		
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
}

static void Tim_callback_row (void const *arg) {
	HAL_GPIO_WritePin(GPIOD, ROW1_PIN | ROW2_PIN | ROW3_PIN | ROW4_PIN, GPIO_PIN_SET); //Desactiva todas las filas
  switch (row) {
		case 0:
				HAL_GPIO_WritePin(GPIOD, ROW1_PIN, GPIO_PIN_RESET);
		    row=1;
				break;
		case 1:
				HAL_GPIO_WritePin(GPIOD, ROW2_PIN, GPIO_PIN_RESET);
		    row=2;
				break;
		case 2:
				HAL_GPIO_WritePin(GPIOD, ROW3_PIN, GPIO_PIN_RESET);
		    row=3;
				break;
		case 3:
				HAL_GPIO_WritePin(GPIOD, ROW4_PIN, GPIO_PIN_RESET);
		    row=0;
				break;
		default:
				break;
	}
}

void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(COL2_PIN);
}

void EXTI9_5_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(COL1_PIN);
	HAL_GPIO_EXTI_IRQHandler(COL3_PIN);
}
  
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  osThreadFlagsSet (id_Th_keypad,Pulsacion);
}
static void Timer_Callback_Rebotes (void const *arg) {
		char key=0;
	if(!HAL_GPIO_ReadPin(GPIOD, COL1_PIN)){
		if (!HAL_GPIO_ReadPin(GPIOD, ROW1_PIN)) {
		 key='1';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW2_PIN)) {
		 key='4';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW3_PIN)) {
		 key='7';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW4_PIN)) {
		 key='*';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
	}
	
 if(!HAL_GPIO_ReadPin(GPIOD, COL2_PIN)){
		if (!HAL_GPIO_ReadPin(GPIOD, ROW1_PIN)) {
		 key='2';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW2_PIN)) {
		 key='5';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW3_PIN)) {
		 key='8';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW4_PIN)) {
		 key='0';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
	}

 if(!HAL_GPIO_ReadPin(GPIOD, COL3_PIN)){
		if (!HAL_GPIO_ReadPin(GPIOD, ROW1_PIN)) {
		 key='3';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW2_PIN)) {
		 key='6';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW3_PIN)) {
		 key='9';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStart(tim_id_row, 50U);
		}
		if (!HAL_GPIO_ReadPin(GPIOD, ROW4_PIN)) {
		 key='#';
		 osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
		 osTimerStop(tim_id_row);
		}
	}

}

int Init_TimRebotes (void) {
	osStatus_t status;                           
  exec_Rebotes = 1U;
  tim_id_Rebotes = osTimerNew((osTimerFunc_t)&Timer_Callback_Rebotes, osTimerOnce, &exec_Rebotes, NULL);
  if (tim_id_Rebotes != NULL) {
    status = osTimerStart(tim_id_Rebotes, 100U); 
    if (status != osOK) {
      return -1;
    }
	}
	return(NULL);
}

int Init_Tim_row (void) {
	osStatus_t status;                   
  exec_row = 1U;
  tim_id_row = osTimerNew((osTimerFunc_t)&Tim_callback_row, osTimerPeriodic, &exec_row, NULL);
  if (tim_id_row != NULL) {  
    status = osTimerStart(tim_id_row, 50U); 
    if (status != osOK) {
      return -1;
    }
	}
	return(NULL);
}

static __NO_RETURN void Th_keypad(void *argument){
	GPIO_Init();
	Init_Tim_row();

	while(1){
		osThreadFlagsWait(Pulsacion, osFlagsWaitAny, osWaitForever);
		osTimerStop(tim_id_row);
		Init_TimRebotes();
	  osThreadYield();
	}
}


