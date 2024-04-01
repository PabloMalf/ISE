#ifndef _NFC_H
#define _NFC_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

typedef struct{
	uint8_t sNum [5];
}MSGQUEUE_OBJ_NFC;

int init_Th_nfc(void);
osMessageQueueId_t get_id_MsgQueue_nfc(void);

#endif
