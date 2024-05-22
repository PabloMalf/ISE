#include "stm32f4xx_hal.h"
#include "lcd.h"
#include "Arial12x12.h"
#include "Driver_SPI.h"
#include "cmsis_os2.h"  
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
static uint8_t buffer [512];
uint8_t  positionL1=0;
uint8_t  positionL2=0;
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
static GPIO_InitTypeDef  GPIO_InitStruct;
TIM_HandleTypeDef htim7;

/*** Thread LCD***/
osThreadId_t tid_ThLCD;                        // thread id
void ThLCD (void *argument);                   // thread function

/****** MsgQueue LCD ******/
 typedef struct {                                // object data type
  char Texto[32];
  uint8_t Linea;
} MSGQUEUE_OBJ_t;
 
MSGQUEUE_OBJ_t LCD;

osMessageQueueId_t mid_MsgQueueLCD;                // message queue id
#define MSGQUEUE_OBJECTS 16

/* Funciones ---------------------------------------------------------*/
void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
  uint8_t j, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');
  
  for (j=0; j<12; j++){
    value1=Arial12x12[offset+(j*2)+1];
    value2=Arial12x12[offset+(j*2)+2];
		
    if(line==1){
      buffer[j+positionL1]=value1;
      buffer[j+128+positionL1]=value2;
		}
		if(line==2){
		  buffer[j+256+positionL2]=value1;
      buffer[j+384+positionL2]=value2;
		}
  }
	 if(line==1)
    positionL1=positionL1+Arial12x12[offset];
	 if(line==2)
	  positionL2=positionL2+Arial12x12[offset];
}

void Borrar_Linea(uint8_t line){
	if(line==1){
		positionL1=0;
		for(int i=0; i<256;i++){
			buffer[i]=0;
		}
	}
	
	if(line==2){
		positionL2=0;
		for(int i=256; i<512;i++){
			buffer[i]=0;
		}
	}
	
}

void symbolToLocalBuffer_RTC(uint8_t line, char buffer[]){
  	if(line==1){
      for(int i=0; strlen(buffer);i++){
      symbolToLocalBuffer(buffer[i],1);
      }
    }
    else{//(line==2)
      for(int i=0; strlen(buffer);i++){
      symbolToLocalBuffer(buffer[i],2);
      }

    }
}


void SPI_Init(){
	
  __SPI1_CLK_ENABLE();  
  /* Initialize the SPI driver */
  SPIdrv->Initialize(NULL);
  /* Power up the SPI peripheral */
  SPIdrv->PowerControl(ARM_POWER_FULL);
  /* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);

}

void GPIO_Init(){
	 //Configuracion de pines
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  
	/*Configure GPIO pin : PA6 - Reset */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*Configure GPIO pin : PD14 - CS */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  /*Configure GPIO pin : PF13 - A0 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}


void LCD_reset(void){
	SPI_Init();
	GPIO_Init();
	
	//Pulso de reset para el LCD(PA6) de una anchura de 1 us
  HAL_GPIO_WritePin ( GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	
	// retardo de 1 ms antes de comenzar la secuencia de comandos de inicialización del LCD.
	delay(1);
	//delay();
	HAL_GPIO_WritePin ( GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay(1000);
  
}
void delay(uint32_t n_microsegundos)
{
  __HAL_RCC_TIM7_CLK_ENABLE();
  
  htim7.Instance         = TIM7;
  htim7.Init.Prescaler   = 83;
	htim7.Init.Period      = n_microsegundos - 1;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start(&htim7);
	
	while (!__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE));
		
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
		
	HAL_TIM_Base_DeInit(&htim7);
	HAL_TIM_Base_Stop(&htim7);
}




void LCD_wr_data(unsigned char data)
{
  ARM_SPI_STATUS stat;
 // Seleccionar CS = 0;
 HAL_GPIO_WritePin ( GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

 // Seleccionar A0 = 1;
 HAL_GPIO_WritePin ( GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

 // Escribir un dato (data) usando la función SPIDrv->Send(…);
   SPIdrv->Send(&data, sizeof(data));

 // Esperar a que se libere el bus SPI;
  do
  {
  stat = SPIdrv->GetStatus();
  }
  while (stat.busy);

 // Seleccionar CS = 1;
 HAL_GPIO_WritePin ( GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

}

void LCD_wr_cmd(unsigned char cmd)
{
  ARM_SPI_STATUS stat;
 // Seleccionar CS = 0;
 HAL_GPIO_WritePin ( GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 // Seleccionar A0 = 0;
 HAL_GPIO_WritePin ( GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

 // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&cmd, sizeof(cmd));
 // Esperar a que se libere el bus SPI;
  do
  {
  stat = SPIdrv->GetStatus();
  }
  while (stat.busy);

 // Seleccionar CS = 1;
 HAL_GPIO_WritePin ( GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_init(void){
  LCD_wr_cmd(0xAE); //Display off
  LCD_wr_cmd(0xA2); //Fija el valor de la relación de la tensión de polarización del LCD a 1/9
  LCD_wr_cmd(0xA0); //El direccionamiento de la RAM de datos del display es la normal
  LCD_wr_cmd(0xC8); //El scan en las salidas COM es el normal
  LCD_wr_cmd(0x22); //Fija la relación de resistencias interna a 2
  LCD_wr_cmd(0x2F); //Power on
  LCD_wr_cmd(0x40); //Display empieza en la línea 0
  LCD_wr_cmd(0xAF); //Display ON
  LCD_wr_cmd(0x81); //Contraste
  LCD_wr_cmd(0x17); //Valor Contraste
  LCD_wr_cmd(0xA4); //Display all points normal
  LCD_wr_cmd(0xA6); //LCD Display normal
}

void LCD_update(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }
}

int Init_LCD (void) {
	
	// Thread LCD
 
  tid_ThLCD = osThreadNew(ThLCD, NULL, NULL);
  if (tid_ThLCD == NULL) {
    return(-1);
  }
	
			// MsgQueueLCD
	  mid_MsgQueueLCD = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
  if (mid_MsgQueueLCD == NULL) {
    ; // Message Queue object not created, handle failure
  }
	
  return(0);
}

 // Thread LCD
void ThLCD (void *argument) {
	osStatus_t status;
  while (1) {
	
  status = osMessageQueueGet(mid_MsgQueueLCD, &LCD, 0, osWaitForever);   // wait for message
		 if (status == osOK){ 
		  LCD_reset();
	    LCD_init();
		  Borrar_Linea(LCD.Linea);
	    for (int i=0; i<strlen(LCD.Texto); i++){
		  symbolToLocalBuffer(LCD.Linea,LCD.Texto[i]);
	   }

	   LCD_update();
			 
		 }
    osThreadYield();                            // suspend thread
  }
}

void LCD_all(void)
{
  Init_LCD();
  SPI_Init();
  GPIO_Init();
  LCD_reset();
  LCD_init();
  LCD_update();
}


