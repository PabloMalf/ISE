#include "camera.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "Driver_I2C.h"

#define CAMERA_I2C_ADDR         0x21    /* 7-bit digital camera I2C address  */

/* DMA source address */
#define DCMI_SRC_ADDR   0x50050028      /* DMA source address */ // COMPROBAR DATO

extern ARM_DRIVER_I2C						Driver_I2C2; 
static ARM_DRIVER_I2C *I2C2drv = &Driver_I2C2;

static uint32_t           FrameBufAddr;
static DMA_HandleTypeDef  hdma_dcmi;
static DCMI_HandleTypeDef hdcmi;


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

static CAMERA_REG Camera_RegInit[] = {
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

   // Scalling numbers
   {0x70, 0x3a},		//X_SCALING
   {0x71, 0x35},		//Y_SCALING
   {0x72, 0x11},		//DCW_SCALING
   {0x73, 0xf0},		//PCLK_DIV_SCALING
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
   {0x4b, 0x01}
};

/**
  \fn          int32_t Camera_Read (uint8_t reg, uint8_t *val)
  \brief       Read value from Camera register
  \param[in]   reg    Register address
  \param[out]  val    Pointer where data will be read from register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
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

/**
  \fn          int32_t Camera_WriteData (uint8_t reg, const uint8_t *val)
  \brief       Write value to Camera register
  \param[in]   reg    Register address
  \param[in]   val    Pointer with data to write to register
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t Camera_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];

  data[0] = reg;
  data[1] = val;
  I2C2drv->MasterTransmit(CAMERA_I2C_ADDR, data, 2, false);
  while (I2C2drv->GetStatus().busy);
  if (I2C2drv->GetDataCount() != 2) return -1;

  return 0;
}


/**
  \fn          void Camera_SetQVGA (void)
  \brief       Configure display size to QVGA (240*320)
*/
static void Camera_SetQVGA (void) {
  uint32_t i;

  for (i = 0; i < (sizeof(Camera_RegInit)/sizeof(CAMERA_REG)); i++) {
    Camera_Write(Camera_RegInit[i].addr, Camera_RegInit[i].val);
  }
}


/**
  \fn          int32_t Camera_Initialize (uint32_t frame_buf_addr)
  \brief       Initialize digital camera
  \param[in]   frame_buf_addr  Framebuffer address
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/

int32_t Camera_Initialize (uint32_t frame_buf_addr) {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t val;
  
  FrameBufAddr = frame_buf_addr;

  I2C2drv->Initialize  (NULL);
  I2C2drv->PowerControl(ARM_POWER_FULL);
  I2C2drv->Control     (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2C2drv->Control     (ARM_I2C_BUS_CLEAR, 0);

  /* Configure camera size */
  Camera_SetQVGA();
  
  Camera_Read (0x6F, &val);   // CAMBIAR ESTE REGISTRO
  val &= ~(1 << 7);
  Camera_Write(0x6F,  val);   // CAMBIAR ESTE REGISTRO

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
     PG9  -> DCMI_VSYNC (VS)
  */
	
  GPIO_InitStruct.Pin = GPIO_PIN_4  | GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GPIO_PIN_5;
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

  return 0;
}

/**
  \fn          int32_t Camera_Uninitialize (void)
  \brief       De-initialize digital camera
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Camera_Uninitialize (void) {

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
     PG9  -> DCMI_VSYNC (VS)
  */
	
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_6);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8 |
                      	 GPIO_PIN_9  | GPIO_PIN_11);
  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5 | GPIO_PIN_6);
  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

  /* Disable DCMI clock */
  __DCMI_CLK_DISABLE();

  return 0;
}

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

/**
  \fn          int32_t Camera_On (void)
  \brief       Turn on digital camera
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Camera_On (void) {
  uint8_t val;

  if (Camera_Read(0x0E, &val) == 0) {  // CAMBIAR ESTE REGISTRO
    val &= ~(1 << 3);

    if (Camera_Write(0x0E, val) == 0) { /* Put camera into normal mode        */  // CAMBIAR ESTE REGISTRO
      HAL_DMA_Start(&hdma_dcmi, DCMI_SRC_ADDR, FrameBufAddr, 1);
      __HAL_DCMI_ENABLE (&hdcmi);
      HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_CONTINUOUS, FrameBufAddr, 1);
      return 0;
    }
  }
  return -1;
}

/**
  \fn          int32_t Camera_Off (void)
  \brief       Turn off digital camera
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Camera_Off (void) {
  uint8_t val;

  HAL_DMA_Abort(&hdma_dcmi);
  __HAL_DCMI_DISABLE (&hdcmi);
  HAL_DCMI_Stop (&hdcmi);

  if (Camera_Read(0x0E, &val) == 0) {               // CAMBIAR ESTE REGISTRO
    /* Put camera into sleep mode */
    if (Camera_Write(0x0E, val | (1<<3)) == 0) {    // CAMBIAR ESTE REGISTRO
      return 0;
    }
  }
  return -1;
}


static __NO_RETURN void Th_camera(void *argument){
	//GPIO_Init();
	uint32_t frame_buf_addr;
	MCO1_Init();
	Camera_Initialize(frame_buf_addr);
	Camera_On();

	while(1){
    // Espera a que se complete la captura (ajusta el tiempo )
    osDelay(1000); 
		
		//Supongo que aqui se almacena la imagen
        
    
    Camera_On();
	  osThreadYield(); // suspend thread 
	}
}
