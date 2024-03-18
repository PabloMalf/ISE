#include "main.h"					
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"		// Solo hace falta definirla donde se vaya a usar o sino dara errores innecesarios
#include "string.h"				// Usamos esta libreria para usar strlen
#include "stdio.h"				// Enunciado del ejercicio, queremos usar sprintf()
#include "cmsis_os2.h"
#include "lcd.h"					// Obviamente hay que incluir su propia libreria

/*******************************************
			Cabecera de las funciones
*******************************************/
static void Reset_Signal(void);
static void SPI_Init(void);
static void LCD_wr_data(unsigned char data);
static void LCD_wr_cmd(unsigned char cmd);

/*******************************************
							Variable
*******************************************/
extern ARM_DRIVER_SPI Driver_SPI1;		// Lo definimos aqui porque usamos la libreria "Driver_SPI.h" en este fichero
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

ARM_SPI_STATUS state;

static GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef htim7;			// Recordar añadir el TIM para que no haya errores

unsigned char buffer[512];

/*******************************************
				Array de las figuras
*******************************************/
static void Reset_Signal(void)
	{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay(1000);
}

void LCD_reset(void)
	{
	
	SPI_Init();
	
	__HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;	//Pin de datos
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = GPIO_PIN_6;		//Pin del reset
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	GPIO_InitStruct.Pin = GPIO_PIN_14;	//Pin del CS
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  //Inicializacion de los pines, con GPIO_PIN_RESET lo limpiamos los flags
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	delay(1000);

	Reset_Signal();

}

static void SPI_Init(void)
	{
	__SPI1_CLK_ENABLE();
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
																																																								
}

void delay (uint32_t microsegundos)
	{
	// Configuracion del timer 7

	__HAL_RCC_TIM7_CLK_ENABLE();
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 84-1;
	htim7.Init.Period = microsegundos-1;
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start(&htim7);
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
	
	// Esperamos a que se active el flag
	while(__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)==0);
	
	// Borramos el flag
	__HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
	
	//Paramos y deshabilitamos el timer para la siguiente llamada a la funcion
	HAL_TIM_Base_Stop(&htim7);
	HAL_TIM_Base_DeInit(&htim7);
}

static void LCD_wr_data(unsigned char data)
	{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

	SPIdrv->Send(&data,sizeof(data));		//Sizeof indica el tamaño de los datos
	
	state = SPIdrv->GetStatus();
	
	while(state.busy){		//Cuando no este ocupado sale del bucle y se libera del bus SPI
		state = SPIdrv->GetStatus();
	}
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}


static void LCD_wr_cmd(unsigned char cmd)
	{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

	SPIdrv->Send(&cmd,sizeof(cmd));		//Sizeof indica el tamaño de los datos
	
	state = SPIdrv->GetStatus();
	
	while(state.busy){		//Cuando no este ocupado sale del bucle y se libera del bus SPI
		state = SPIdrv->GetStatus();
	}
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
void LCD_Init(void)				// El de los comandos
	{
	LCD_wr_cmd(0xAE);		//Display off
	
	LCD_wr_cmd(0xA2);		//Fija el valor de la relacion de la tenson de polarizacion del LCD a 1/9
	
	LCD_wr_cmd(0xA0);		//El direccionamiento de la RAM de datos del display es la normal
	
	LCD_wr_cmd(0xC8);		//El scan en las salidas COM es el normal
	
	LCD_wr_cmd(0x22);		//Fija la relacion de resistencias interna a 2
	
	LCD_wr_cmd(0x2F);		//Power on
	
	LCD_wr_cmd(0x40);		//Display empieza en la linea 0
	
	LCD_wr_cmd(0xAF);		//Display ON
	
	LCD_wr_cmd(0x81);		//Contraste
	
	LCD_wr_cmd(0x17);		//Valor contraste (A libre eleccion)
	
	LCD_wr_cmd(0xA4);		//Display all points normal
	
	LCD_wr_cmd(0xA6);		//LCD_Init Display normal
}

void LCD_update(void) 
	{ 
    int i; 
    LCD_wr_cmd(0x00);      // 4 bits de la parte baja de la dirección a 0 
    LCD_wr_cmd(0x10);      // 4 bits de la parte alta de la dirección a 0 
    LCD_wr_cmd(0xB0);      // Página 0 
    
    for(i=0;i<128;i++){ 
        LCD_wr_data(buffer[i]); 
        } 
  
     
    LCD_wr_cmd(0x00);      // 4 bits de la parte baja de la dirección a 0 
    LCD_wr_cmd(0x10);      // 4 bits de la parte alta de la dirección a 0 
    LCD_wr_cmd(0xB1);      // Página 1 
     
    for(i=128;i<256;i++){ 
        LCD_wr_data(buffer[i]); 
        } 
    
    LCD_wr_cmd(0x00);       
    LCD_wr_cmd(0x10);      
    LCD_wr_cmd(0xB2);      //Página 2 
 
 
    for(i=256;i<384;i++){ 
        LCD_wr_data(buffer[i]); 
        } 
    
    LCD_wr_cmd(0x00);       
    LCD_wr_cmd(0x10);       
    LCD_wr_cmd(0xB3);      // Pagina 3 
     
     
    for(i=384;i<512;i++){ 
        LCD_wr_data(buffer[i]); 
        } 
 
} 
	
void LCD_symbolToLocalBuffer_L1(char symbol[], int size)
	{
	uint8_t i, j, value1, value2;
	uint16_t offset=0;
	uint8_t positionL1 = 0;	
		
	for(j=0; j< size; j++){
	offset=25*(symbol[j] - ' ');
	
		for(i=0; i<12; i++){
		
			value1=Arial12x12[offset+i*2+1];
			value2=Arial12x12[offset+i*2+2];
		
			buffer[i+positionL1]=value1;					
			buffer[i+128+positionL1]=value2;
		}
	positionL1=positionL1+Arial12x12[offset];
	}
	LCD_update();
}
void LCD_symbolToLocalBuffer_L2(char symbol[], int size)
	{
	uint8_t i, j, value1, value2;
	uint16_t offset=0;
	uint8_t positionL1 = 0;
		
	for(j=0; j< size; j++){
	offset=25*(symbol[j] - ' ');
	
		for(i=0; i<12; i++){
		
			value1=Arial12x12[offset+i*2+1];
			value2=Arial12x12[offset+i*2+2];
		
			buffer[i+128*2+positionL1]=value1;					
			buffer[i+128*3+positionL1]=value2;
		}
	positionL1=positionL1+Arial12x12[offset];
	}
	LCD_update();
}
	
void symbolToLocalBuffer(uint8_t line, char symbol[], int size)
  {
	if (line == 1){
	LCD_symbolToLocalBuffer_L1(symbol, size);
	}
	else if(line == 2){
	LCD_symbolToLocalBuffer_L2(symbol, size);
	}
}

void LCD_clear(void)
  {
	for(int i=0; i<512;i++){
		buffer[i]=0x00;
	}	
	LCD_update();
}
