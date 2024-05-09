#ifndef _NFC_H
#define _NFC_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define NFC_FLAG_ON		1U << 0
#define NFC_FLAG_OFF	1U << 1

typedef struct{
	uint8_t sNum [5];
}MSGQUEUE_OBJ_NFC;

int init_Th_nfc(void);
osMessageQueueId_t get_id_MsgQueue_nfc(void);

#endif
