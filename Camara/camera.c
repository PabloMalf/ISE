#include "camera.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
//#include "stm32f4xx_rcc.h"

/*** Thread Camera ***/
static osThreadId_t id_Th_camera;
static void Th_camera(void *arg);
int init_Th_camera(void);

/*** Queue camera ***/
static osMessageQueueId_t id_MsgQueue_camera;
static int Init_MsgQueue_camera(void);

/*** Functions camera ***/
//static void	GPIO_Init(void);
static void MCO1_Init(void);


static int Init_MsgQueue_camera(){
	id_MsgQueue_camera = osMessageQueueNew(MSGQUEUE_OBJECTS_CAMERA, sizeof(MSGQUEUE_OBJ_CAMERA), NULL);
	if(id_MsgQueue_camera == NULL)
		return (-1); 
	return(0);
}


int init_Th_camera(void){
	id_Th_camera = osThreadNew(Th_camera, NULL, NULL);
	if(id_Th_camera == NULL)
		return(-1);
	return(Init_MsgQueue_camera());
}


osMessageQueueId_t get_id_MsgQueue_camera(void){
	return id_MsgQueue_camera;
}

/*void GPIO_Init(){
	static GPIO_InitTypeDef  GPIO_InitStruct;
	 //Configuracion de pines
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
	Configure GPIO pin
			*/

 /* GPIO_InitStruct.Pin = COL1_PIN | COL2_PIN | COL3_PIN;
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
	
}*/

void MCO1_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;
 __HAL_RCC_GPIOA_CLK_ENABLE(); //Enable clock to GPIOA
	
 /*********************
   PA.8 Configuration
 *********************/
  
 /* PA8 will be used for clock output (MCO1) and not
    as GPIO so configure it for alternate funciton
 */

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;// GPIO_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO; //Duda
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1); //8Mhz
	//Segun el codigo la f es 8Mhz, en el pulseviewer aparece 4Mhz.
	//Seguramente sea problema del pulseviewer. (Probar en clase)
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);
}


static __NO_RETURN void Th_camera(void *argument){
	//GPIO_Init();
	MCO1_Init();

	while(1){
		osDelay(100);
	  osThreadYield(); // suspend thread 
	}
}
