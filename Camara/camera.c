#include "camera.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "Driver_I2C.h"

#define CAMERA_I2C_ADDR         0x21    /* 7-bit digital camera I2C address  */

/** I2C **/
extern ARM_DRIVER_I2C						Driver_I2C2; 
static ARM_DRIVER_I2C *I2C2drv = &Driver_I2C2;

static DCMI_HandleTypeDef hdcmi;     //DCMI
static DMA_HandleTypeDef  hdma_dcmi; //DMA

/* Camera register structure */
typedef struct { 
  uint8_t addr;                         /* Register address */
  uint8_t val;                          /* Register value   */
} CAMERA_REG;

/*** Thread Camera ***/
static osThreadId_t id_Th_camera;
static void Th_camera(void *arg);
int init_Th_camera(void);


/*** Queue camera ***/
static osMessageQueueId_t id_MsgQueue_camera;
static int Init_MsgQueue_camera(void);


/** Timer2 Channel4 genera le relloj de entrada XLK**/
TIM_HandleTypeDef htim2;
TIM_OC_InitTypeDef sConfig;

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

/*** Functions camera ***/
static void TimerXLK_Init(void);
static void DCMI_DMA_init(void);
static void I2C_Init(void);
static int32_t Camera_Write (uint8_t reg, uint8_t val);
static int32_t Camera_Read (uint8_t reg, uint8_t *val);


static void TimerXLK_Init(void){
  //Pin
  GPIO_InitTypeDef GPIO_InitStruct; 
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate= GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /**/
  
  __HAL_RCC_TIM2_CLK_ENABLE();
  htim2.Instance =TIM2;
	htim2.Init.Prescaler =1;  //84Mhz/420 =200000Hz
  htim2.Init.Period = 3; //20000Hz /200 =1000Hz
  HAL_TIM_OC_Init(&htim2);
  
    //Rellenar sConfig
  sConfig.OCMode=TIM_OCMODE_PWM1;
  sConfig.Pulse=2;
  sConfig.OCPolarity=TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode=TIM_OCFAST_DISABLE;
  
  
	
 // Enable the TIM2 peripheral
  
  HAL_TIM_OC_ConfigChannel(&htim2,&sConfig,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
}


static void DCMI_DMA_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();	
  __GPIOB_CLK_ENABLE();		
  __GPIOC_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOG_CLK_ENABLE();
	
	/* DCMI GPIO Configuration:
   PA4  -> DCMI_HSYNC (HS)
	 PA6  -> DCMI_PIXCK (PLK)
   PB6  -> DCMI_D5    (D5)
   PC6  -> DCMI_D0    (D0)
   PC7  -> DCMI_D1    (D1)
   PC8  -> DCMI_D2    (D2)
   PC9  -> DCMI_D3    (D3)
   PC11 -> DCMI_D4    (D4)
   PE5  -> DCMI_D6    (D6)
   PE6  -> DCMI_D7    (D7)		 
   PG9  -> DCMI_VSYNC (VS)*/

  GPIO_InitStruct.Pin = GPIO_PIN_4  | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8 |
                      	GPIO_PIN_9  | GPIO_PIN_11; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5  | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
  /* Enable DCMI and DMA peripheral clock */
  __DCMI_CLK_ENABLE();
  __DMA2_CLK_ENABLE();
	
  /* Configure DMA */
  hdma_dcmi.Instance = DMA2_Stream1;
  hdma_dcmi.Init.Channel = DMA_CHANNEL_1;
  hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dcmi.Init.MemInc = DMA_MINC_DISABLE;
  hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_dcmi.Init.Mode = DMA_CIRCULAR;
  hdma_dcmi.Init.Priority = DMA_PRIORITY_LOW;
  hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
  hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
  HAL_DMA_Init(&hdma_dcmi);
	
  /* Configure DCMI peripheral */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  HAL_DCMI_Init(&hdcmi);
}

static void I2C_Init(void){
	
  I2C2drv->Initialize  (NULL);
  I2C2drv->PowerControl(ARM_POWER_FULL);
  I2C2drv->Control     (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2C2drv->Control     (ARM_I2C_BUS_CLEAR, 0);
}
static int32_t Camera_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];

  data[0] = reg;
  data[1] = val;
  I2C2drv->MasterTransmit(CAMERA_I2C_ADDR, data, 2, false);
  while (I2C2drv->GetStatus().busy);
  if (I2C2drv->GetDataCount() != 2) return -1;

  return 0;
}

static int32_t Camera_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];

  data[0] = reg;
  I2C2drv->MasterTransmit(CAMERA_I2C_ADDR, data, 1, true);
  while (I2C2drv->GetStatus().busy);
  if (I2C2drv->GetDataCount() != 1) return -1;
  I2C2drv->MasterReceive (CAMERA_I2C_ADDR, val, 1, false);
  while (I2C2drv->GetStatus().busy);
  if (I2C2drv->GetDataCount() != 1) return -1;

  return 0;
}


static void Th_camera(void *argument){
  TimerXLK_Init();
	DCMI_DMA_init();
	I2C_Init();
  //HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4); //No debe ser necesario aqui
  

	while(1){
    // Espera a que se complete la captura (ajusta el tiempo )
    osDelay(1000); 
    
		//Supongo que aqui se almacena la imagen
        
	  //osThreadYield(); // suspend thread 
	}
}






