#ifndef _RC522_H
#define _RC522_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define MSGQUEUE_OBJECTS_NFC 4

#define MFRC522_CS_PORT		GPIOA
#define MFRC522_CS_PIN		GPIO_PIN_15
#define MFRC522_RST_PORT	GPIOB
#define MFRC522_RST_PIN		GPIO_PIN_12

typedef struct{
	int a;
}MSGQUEUE_OBJ_NFC;

int init_Th_nfc(void);
osMessageQueueId_t get_id_MsgQueue_nfc(void);

#endif
