#include "keypad.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"


/*** Thread Keypad ***/
static osThreadId_t id_Th_keypad;
static void Th_keypad(void *arg);
int init_Th_keypad(void);

/*** Queue Keypad ***/
static osMessageQueueId_t id_MsgQueue_keypad;
static int Init_MsgQueue_keypad(void);

/*** Functions Keypad ***/
static void	GPIO_Init(void);
static void read_keypad(void);


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
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ROW1_PIN | ROW2_PIN |
	                      ROW3_PIN | ROW4_PIN;	 
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP ;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}


void read_keypad(void){
	
	char key=0;
	/* Row 1 LOW and alls other Rows HIGH*/
	HAL_GPIO_WritePin (GPIOD, ROW1_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOD, ROW2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW4_PIN, GPIO_PIN_SET);
	
	if (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN));
		key='1';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
	else if (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN));
		key='2';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
	else if(!HAL_GPIO_ReadPin(GPIOD, COL3_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN));
		key='3';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }

	/* Row 2 LOW and alls other Rows HIGH*/
	HAL_GPIO_WritePin (GPIOD, ROW1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW2_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOD, ROW3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW4_PIN, GPIO_PIN_SET);
	
	if (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN));
		key='4';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	}
	
  else if (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN));
		key='5';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	}
	
  else if (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN));
		key='6';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }	 
  
	 /* Row 3 LOW and alls other Rows HIGH*/
	HAL_GPIO_WritePin (GPIOD, ROW1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW3_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (GPIOD, ROW4_PIN, GPIO_PIN_SET);
	
	if (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN));
		key='7';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
  else if (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN));
		key='8';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
  else if (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN));
		key='9';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
	/* Row 1 LOW and alls other Rows HIGH*/
	HAL_GPIO_WritePin (GPIOD, ROW1_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW3_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin (GPIOD, ROW4_PIN, GPIO_PIN_RESET);
	
	if (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL1_PIN));
		key='*';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
  else if (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL2_PIN));
		key='0';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
  else if(!HAL_GPIO_ReadPin(GPIOD, COL3_PIN)) {
	 while (!HAL_GPIO_ReadPin(GPIOD, COL3_PIN));
		key='#';
		osMessageQueuePut(id_MsgQueue_keypad, &key, 0, 0);
	 }
	
}

static __NO_RETURN void Th_keypad(void *argument){
	GPIO_Init();

	while(1){
		read_keypad();
		osDelay(100);
	  osThreadYield(); // suspend thread 
	}
}