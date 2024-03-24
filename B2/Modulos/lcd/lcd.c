#include "lcd.h"
#include <stdio.h>
#include "Driver_I2C.h"
#include "stm32f4xx_hal.h"

#define I2C_ADDR 0x27 //<<1 0x3F

#define RS_BIT	1U << 9
#define WR_BIT	1U << 8
#define D7_BIT	1U << 7
#define D6_BIT 	1U << 6
#define D5_BIT	1U << 5
#define D4_BIT 	1U << 4
#define D3_BIT	1U << 3
#define D2_BIT 	1U << 2
#define D1_BIT	1U << 1
#define D0_BIT 	1U << 0

#define LCD_ROWS 4
#define LCD_COLS 20

extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

static osThreadId_t id_Th_lcd;
static osMessageQueueId_t id_MsgQueue_lcd;

//OS MANAGE
int init_Th_lcd(void);
static void Th_lcd(void *arg);
static int Init_MsgQueue_lcd(void);

//LCD MANAGE
static void LCD_init(void);
static void LCD_wr_cmd (uint8_t cmd);
static void LCD_wr_data(uint8_t data);

//MID LEVEL
static void callback_i2c(uint32_t event);



static void LCD_init(void){
	I2Cdrv-> Initialize	 (callback_i2c);
	I2Cdrv-> PowerControl (ARM_POWER_FULL);
	I2Cdrv-> Control			(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	I2Cdrv-> Control			(ARM_I2C_BUS_CLEAR, 0);
	
	osDelay(16*MS);
	//write 00 0011 xxxx
	//osDelay(
}



//-------------------------------------------------------


static int Init_MsgQueue_lcd(){
	id_MsgQueue_lcd = osMessageQueueNew(MSGQUEUE_OBJECTS_LCD, sizeof(MSGQUEUE_OBJ_LCD), NULL);
	if(id_MsgQueue_lcd == NULL)
		return (-1); 
	return(0);
}


int init_Th_lcd(void){
	id_Th_lcd = osThreadNew(Th_lcd, NULL, NULL);
	if(id_Th_lcd == NULL)
		return(-1);
	return(Init_MsgQueue_lcd());
}


osMessageQueueId_t get_id_MsgQueue_lcd(void){
	return id_MsgQueue_lcd;
}


static void callback_i2c(uint32_t event){
	osThreadFlagsSet(id_Th_lcd, event);
}


static void Get_I2C_ADDR(void){
	uint8_t addr;
	uint32_t flags;
	for(addr = 1; addr < 127; addr++ ){
		I2Cdrv->MasterTransmit(addr, NULL, NULL, true);
		flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
		printf("%d", flags);
	}
}

static void Th_lcd(void *argument){
	LCD_init();
	Get_I2C_ADDR();
	
	/*
	MSGQUEUE_OBJ_LCD msg_lcd;
	
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_lcd, &msg_lcd, NULL, osWaitForever)){
			clear_lcd();
			set_cursor(0, 0);
			write_string(msg_lcd.L0);
			set_cursor(1, 0);
			write_string(msg_lcd.L1);
			set_cursor(2, 0);
			write_string(msg_lcd.L2);
			set_cursor(3, 0);
			write_string(msg_lcd.L3);
		}
	}*/
}














