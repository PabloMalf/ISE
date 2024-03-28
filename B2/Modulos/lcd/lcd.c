#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "Driver_I2C.h"
#include "stm32f4xx_hal.h"

#define SLAVE_ADDRESS_LCD 0x27

extern ARM_DRIVER_I2C						Driver_I2C1; 
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

static osThreadId_t id_Th_lcd;
static osMessageQueueId_t id_MsgQueue_lcd;

int init_Th_lcd(void);
static void Th_lcd(void *arg);
static int Init_MsgQueue_lcd(void);

static uint8_t back_light = 0x08;

static void send_cmd(char cmd);
static void send_data(char data);
static void callback_i2c(uint32_t event);
static void send_string(char *str);
static void set_cursor(int row, int col);
static void clear(void);

static int Init_MsgQueue_lcd(){
	id_MsgQueue_lcd = osMessageQueueNew(MSGQUEUE_OBJECTS_LCD, sizeof(MSGQUEUE_OBJ_LCD), NULL);
	if(id_MsgQueue_lcd == NULL)
		return(-1); 
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

static void init_lcd(void){
	I2Cdrv->Initialize	(callback_i2c);
	I2Cdrv->PowerControl(ARM_POWER_FULL);
	I2Cdrv->Control			(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	
	osDelay(50);	// wait for >40ms
	send_cmd(0x30);
	osDelay(5);	// wait for >4.1ms
	send_cmd(0x30);
	osDelay(1);	// wait for >100us
	send_cmd(0x30);
	osDelay(1); //prev 10
	send_cmd(0x20);	// 4bit mode
	osDelay(1); //prev 10
	
	send_cmd(0x28); // Function set --> DL=0(4 bit mode), N = 1(2 line display) F = 0(5x8 characters)
	osDelay(1);
	send_cmd(0x08); //Display on/off control --> D=0,C=0, B=0	---> display off
	osDelay(1);
	send_cmd(0x01);	// clear display
	osDelay(2);
	send_cmd(0x06); //Entry mode set --> I/D = 1(increment cursor) & S = 0(no shift)
	osDelay(1);
	send_cmd(0x0C); //Display on/off control --> D = 1, C and B = 0.(Cursor and blink, last two bits)
}

static void send_cmd(char cmd){
	char data_u, data_l;
	uint32_t flags;
	int error;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l =((cmd << 4) & 0xf0);
	
	data_t[0] = data_u | back_light | 0x04;	//en=1, rs=0
	data_t[1] = data_u | back_light | 0x00;	//en=0, rs=0
	data_t[2] = data_l | back_light | 0x04;	//en=1, rs=0	
	data_t[3] = data_l | back_light | 0x00;	//en=0, rs=0
	error = I2Cdrv->MasterTransmit(SLAVE_ADDRESS_LCD,(uint8_t *)data_t, 4, false);
	flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
	printf("Error: %d \tFlags: %d \tCMD: %x\n", error, flags, cmd);
}

static void send_data(char data){
	char data_u, data_l;
	uint32_t flags;
	int error;
	
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l =((data << 4) & 0xf0);
	
	data_t[0] = data_u | back_light | 0x05;	//en=1, rs=1  0101
	data_t[1] = data_u | back_light | 0x01;	//en=0, rs=1	0001
	data_t[2] = data_l | back_light | 0x05;	//en=1, rs=1
	data_t[3] = data_l | back_light | 0x01;	//en=0, rs=1
	error = I2Cdrv->MasterTransmit(SLAVE_ADDRESS_LCD,(uint8_t *)data_t, 4, false);
	flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
	printf("Error: %d \tFlags: %d \tData: %x\n", error, flags, data);
}

static void send_string(char *str){
	while(*str){
		send_data(*str++);
	}
	printf("");
}

static void set_cursor(int row, int col){
	
	switch(row){
		case 0: col |= 0x80; break;
		case 1: col |= 0xC0; break;
		case 2: col |= 0x94; break;
		case 3: col |= 0xD4; break;
	}
	send_cmd(col);
	osDelay(1);
}

static void clear(void){
	send_cmd(0x01);
	osDelay(2);
}

//static void test_get_addr(void){
//	uint32_t addr, flags = 0x00;
//	int error = 0;
//	uint8_t data = 0x00;
//	
//	I2Cdrv->Initialize	(callback_i2c);
//	I2Cdrv->PowerControl(ARM_POWER_FULL);
//	I2Cdrv->Control			(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
//	
//	for(addr = 0; addr<127; addr++){
//		error = I2Cdrv->MasterTransmit(addr, &data, 1, true);
//		flags = osThreadFlagsWait(0xFFFF, osFlagsWaitAny, osWaitForever);
//		printf("ADDR: %x \tError: %d\tFlags: %d\n", addr, error, flags);
//		osDelay(150);
//	}
//}

static void Th_lcd(void *argument){
	MSGQUEUE_OBJ_LCD msg_lcd;
	init_lcd();	
	
	while(1){
		if(osOK == osMessageQueueGet(id_MsgQueue_lcd, &msg_lcd, NULL, osWaitForever)){
			back_light = msg_lcd.state;
			clear();
			if(back_light){
				set_cursor(0, 0);
				send_string(msg_lcd.L0);
				set_cursor(1, 0);
				send_string(msg_lcd.L1);
				set_cursor(2, 0);
				send_string(msg_lcd.L2);
				set_cursor(3, 0);
				send_string(msg_lcd.L3);
			}
		}
	}
}
