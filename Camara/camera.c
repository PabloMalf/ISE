#include "camera.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "Driver_I2C.h"
#include "stm32f4xx.h"


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

/** Timer2 Channel4 genera le reloj de entrada XLK**/
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
static void Camera_SetQQVGA (void);
static void Disable_RET_PWDN (void);

static CAMERA_REG Camera_RegInit_qqvga[] = {				
   {0x12, 0x80},		//Reset registers

   // Image format
   {0x12, 0x14},		//QVGA size, RGB mode

   {0x40, 0xd0},		//RGB565
   {0xb0, 0x84},		//Color mode

   // Hardware window
   {0x11, 0x01},		//PCLK settings, 15fps
   {0x32, 0x80},		//HREF
   {0x17, 0x17},		//HSTART
   {0x18, 0x05},		//HSTOP
   {0x03, 0x0a},		//VREF
   {0x19, 0x02},		//VSTART
   {0x1a, 0x7a},		//VSTOP
   /*
      {0x17,0x16},
      {0x18,0x04},
      {0x32,0x24},
      {0x19,0x02},
      {0x1a,0x7a},
      {0x03,0x0a},
      */
   // Scalling numbers
   {0x70, 0x3a},		//X_SCALING
   {0x71, 0x35},		//Y_SCALING
   /*{0x72, 0x11},		//DCW_SCALING
     {0x73, 0xf0},		//PCLK_DIV_SCALING*/
   /*{0x72, 0x11},
     {0x73, 0xf1},*/
   {0x72, 0x22}, //20190712
   {0x73, 0xf2}, //20190712 
   {0xa2, 0x02},		//PCLK_DELAY_SCALING

   // Matrix coefficients
   {0x4f, 0x80},		{0x50, 0x80},
   {0x51, 0x00},		{0x52, 0x22},
   {0x53, 0x5e},		{0x54, 0x80},
   {0x58, 0x9e},

   // Gamma curve values
   {0x7a, 0x20},		{0x7b, 0x10},
   {0x7c, 0x1e},		{0x7d, 0x35},
   {0x7e, 0x5a},		{0x7f, 0x69},
   {0x80, 0x76},		{0x81, 0x80},
   {0x82, 0x88},		{0x83, 0x8f},
   {0x84, 0x96},		{0x85, 0xa3},
   {0x86, 0xaf},		{0x87, 0xc4},
   {0x88, 0xd7},		{0x89, 0xe8},

   // AGC and AEC parameters
   {0xa5, 0x05},		{0xab, 0x07},
   {0x24, 0x95},		{0x25, 0x33},
   {0x26, 0xe3},		{0x9f, 0x78},
   {0xa0, 0x68},		{0xa1, 0x03},
   {0xa6, 0xd8},		{0xa7, 0xd8},
   {0xa8, 0xf0},		{0xa9, 0x90},
   {0xaa, 0x94},		{0x10, 0x00},

   // AWB parameters
   {0x43, 0x0a},		{0x44, 0xf0},
   {0x45, 0x34},		{0x46, 0x58},
   {0x47, 0x28},		{0x48, 0x3a},
   {0x59, 0x88},		{0x5a, 0x88},
   {0x5b, 0x44},		{0x5c, 0x67},
   {0x5d, 0x49},		{0x5e, 0x0e},
   {0x6c, 0x0a},		{0x6d, 0x55},
   {0x6e, 0x11},		{0x6f, 0x9f},

   {0x6a, 0x40},		{0x01, 0x40},
   {0x02, 0x60},		{0x13, 0xe7},

   // Additional parameters
   {0x34, 0x11},		{0x3f, 0x00},
   {0x75, 0x05},		{0x76, 0xe1},
   {0x4c, 0x00},		{0x77, 0x01},
   {0xb8, 0x0a},		{0x41, 0x18},
   {0x3b, 0x12},		{0xa4, 0x88},
   {0x96, 0x00},		{0x97, 0x30},
   {0x98, 0x20},		{0x99, 0x30},
   {0x9a, 0x84},		{0x9b, 0x29},
   {0x9c, 0x03},		{0x9d, 0x4c},
   {0x9e, 0x3f},		{0x78, 0x04},
   {0x0e, 0x61},		{0x0f, 0x4b},
   {0x16, 0x02},		{0x1e, 0x00},
   {0x21, 0x02},		{0x22, 0x91},
   {0x29, 0x07},		{0x33, 0x0b},
   {0x35, 0x0b},		{0x37, 0x1d},
   {0x38, 0x71},		{0x39, 0x2a},
   {0x3c, 0x78},		{0x4d, 0x40},
   {0x4e, 0x20},		{0x69, 0x00},
   {0x6b, 0x3a},		{0x74, 0x10},
   {0x8d, 0x4f},		{0x8e, 0x00},
   {0x8f, 0x00},		{0x90, 0x00},
   {0x91, 0x00},		{0x96, 0x00},
   {0x9a, 0x00},		{0xb1, 0x0c},
   {0xb2, 0x0e},		{0xb3, 0x82},
   {0x4b, 0x01},
   /*
      {0x0c, 0x04},
      {0x3e, 0x19}*/
   {0x3e, 0x1a}, //20190712
   {0x0c, 0x04}, //20190712
};

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
  hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; //DMA_MDATAALIGN_HALFWORD
  hdma_dcmi.Init.Mode = DMA_NORMAL; //DMA_CIRCULAR
  hdma_dcmi.Init.Priority = DMA_PRIORITY_LOW;
  hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  //hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  //hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
  //hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
  //HAL_DMA_Init(&hdma_dcmi);
	
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



static void Camera_SetQQVGA (void) {
  uint32_t i;

  for (i = 0; i < (sizeof(Camera_RegInit_qqvga)/sizeof(CAMERA_REG)); i++) {
    Camera_Write(Camera_RegInit_qqvga[i].addr, Camera_RegInit_qqvga[i].val);
  }
}

static void Disable_RET_PWDN (void){
	/* PC10  -> RET  (Se desabilita a nivel alto)   		 
     PC12  -> PWDN (Se desabilita a nivel bajo)  */
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_10  | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	
}

static void Th_camera(void *argument){
  TimerXLK_Init();
	DCMI_DMA_init();
	I2C_Init();
	Camera_SetQQVGA();
	Disable_RET_PWDN();
  //HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4); //No debe ser necesario aqui
  

	while(1){
    // Espera a que se complete la captura (ajusta el tiempo )
    osDelay(1000); 
		HAL_DCMI_Start_DMA(hdcmi, DCMI_MODE_SNAPSHOT, destAddress, 160 * 120/2);
    
		//Supongo que aqui se almacena la imagen
        
	  //osThreadYield(); // suspend thread 
	}
}






