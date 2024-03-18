#ifndef __LCD_H
#define __LCD_H

/*
	CONEXIONES HARDWARE

	LCD(SPI)			Mbed				NUCLEO-F429ZI
	MOSI			->	DIP5		->	PB5
	Reset			->	DIP6		->	PA6
	SCK				->	DIP7		->	PA5
	A0				->	DIP8		->	PF13
	CS				->	DIP11		->	PD14
*/

void LCD_init(void);
void write_LCD(char data_L1[32], char data_L2[32]);
void write_LCD_L1(char data[32]);
void write_LCD_L2(char data[32]);

#endif
