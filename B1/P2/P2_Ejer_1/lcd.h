#include "stdint.h"		// Hay que incluir esto porque el uint32_t, uint8_t y otros no estan definidos

#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

 void LCD_reset(void);
 void delay(uint32_t microsegundos);
 void LCD_Init(void);
 void LCD_update(void);
 void LCD_symbolToLocalBuffer_L1(char symbol[], int size);
 void LCD_symbolToLocalBuffer_L2(char symbol[], int size);
 void symbolToLocalBuffer(uint8_t line, char symbol[], int size);
 void LCD_clear(void);

#ifdef __cplusplus
}
#endif

#endif
