


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"


/* Exported functions ------------------------------------------------------- */
void SPI_Init(void);
void GPIO_Init(void);
void delay(uint32_t n_microsegundos);

void Borrar_Linea(uint8_t line);
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init(void);
void LCD_update(void);
void symbolToLocalBuffer(uint8_t line, uint8_t symbol);
void LCD_all(void);
void symbolToLocalBuffer_RTC(uint8_t line, char buffer[]);

extern int Init_LCD(void);

#endif /* __LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
