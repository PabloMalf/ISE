#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "cmsis_os2.h"  

#define MSGQUEUE_OBJECTS_RTC 4

typedef struct{
  char   hour[40];
  char   date[40];
} MSGQUEUE_OBJ_RTC;

int init_Th_RTC(void);
osMessageQueueId_t get_id_MsgQueue_RTC(void);

#endif /* __RTC_H */

