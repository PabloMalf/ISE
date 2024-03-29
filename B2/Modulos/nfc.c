//hay que reescribir las funciones de la libreria 	#include <SPI.h> y de la libreria #include <MFRC522.h> de arduino en c
//el sistema para el que hay que reescribirlas esta compuesto por un stm32f429zi, con sistema operativo cmsis rtosv2, driver del spi proporcionado por CMSIS Driver SPI, capa de abstraccion de perifericos de stm32f4 (hal)

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "Driver_SPI.h"

//funciones de la libreria SPI.h de arduino
//funcion de inicializacion del spi

extern ARM_DRIVER_SPI           Driver_SPI1;
static ARM_DRIVER_SPI *SPIdrv = &Driver_SPI1;

static void SPI_Init(void);
static void SPI_Write(uint8_t data);
static uint8_t SPI_Read(void);
static uint8_t SPI_Transfer(uint8_t data);
static void SPI_CallBack(uint32_t event);

//funcion de inicializacion del spi

void SPI_Init(void){
  SPIdrv->Initialize(SPI_CallBack);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW, 1000000);
}

//funcion de escritura de un byte en el spi

void SPI_Write(uint8_t data){
  SPIdrv->Send(&data, 1);
}

//funcion de lectura de un byte en el spi

uint8_t SPI_Read(void){
  uint8_t data;
  SPIdrv->Receive(&data, 1);
  return data;
}

//funcion de escritura y lectura de un byte en el spi

uint8_t SPI_Transfer(uint8_t data){
  uint8_t data_rx;
  SPIdrv->Transfer(&data, &data_rx, 1);
  return data_rx;
}

//funcion de callback del spi

void SPI_CallBack(uint32_t event){
  osThreadFlagsSet(tid_Thread, event);
}

//funciones de la libreria MFRC522.h de arduino
//funcion de inicializacion del mfrc522

void MFRC522_Init(void){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_Delay(50);
  MFRC522_Write(0x01);
  MFRC522_Write(0x0F);
  MFRC522_Write(0x2B);
  MFRC522_Write(0x0C);
  MFRC522_Write(0x0C);
  MFRC522_Write(0x0C);
  MFRC522_Write(0x0C);
  MFRC522_Write(0x0C);

}


uint8_t MFRC522_Read(void){
  uint8_t data;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  SPI_Write(0x30);
  data = SPI_Read();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  return data;
}


void MFRC522_Write(uint8_t data){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  SPI_Write(0xA0);
  SPI_Write(data);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
}


uint8_t MFRC522_Request(uint8_t req_mode){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0D);
  MFRC522_Write(req_mode);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Anticoll(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Select(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Auth(uint8_t auth_mode, uint8_t block_addr, uint8_t *key, uint8_t *uid){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(auth_mode);
  MFRC522_Write(block_addr);
  MFRC522_Write(key[0]);
  MFRC522_Write(key[1]);
  MFRC522_Write(key[2]);
  MFRC522_Write(key[3]);
  MFRC522_Write(uid[0]);
  MFRC522_Write(uid[1]);
  MFRC522_Write(uid[2]);
  MFRC522_Write(uid[3]);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Read_Block(uint8_t block_addr, uint8_t *data){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  MFRC522_Write(block_addr);
  status = MFRC522_Read();
  if(status == 0x04){
    data[0] = MFRC522_Read();
    data[1] = MFRC522_Read();
    data[2] = MFRC522_Read();
    data[3] = MFRC522_Read();
    return 1;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Write_Block(uint8_t block_addr, uint8_t *data){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  MFRC522_Write(block_addr);
  MFRC522_Write(data[0]);
  MFRC522_Write(data[1]);
  MFRC522_Write(data[2]);
  MFRC522_Write(data[3]);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Halt(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Reset(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_CalcCRC(uint8_t *data){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  MFRC522_Write(data[0]);
  MFRC522_Write(data[1]);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Communicate(uint8_t *data){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  MFRC522_Write(data[0]);
  MFRC522_Write(data[1]);
  MFRC522_Write(data[2]);
  MFRC522_Write(data[3]);
  MFRC522_Write(data[4]);
  MFRC522_Write(data[5]);
  MFRC522_Write(data[6]);
  MFRC522_Write(data[7]);
  MFRC522_Write(data[8]);
  MFRC522_Write(data[9]);
  MFRC522_Write(data[10]);
  MFRC522_Write(data[11]);
  MFRC522_Write(data[12]);
  MFRC522_Write(data[13]);
  MFRC522_Write(data[14]);
  MFRC522_Write(data[15]);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Calibrate(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}


uint8_t MFRC522_Amplitude(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}

uint8_t MFRC522_Demod(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}

uint8_t MFRC522_CalcCRC_Preset(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}

uint8_t MFRC522_CalcCRC_Result(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}

uint8_t MFRC522_CalcCRC_Preset_Result(void){
  uint8_t status;
  uint8_t back_data;
  MFRC522_Write(0x0C);
  MFRC522_Write(0x93);
  status = MFRC522_Read();
  if(status == 0x04){
    back_data = MFRC522_Read();
    return back_data;
  }
  else{
    return 0;
  }
}
