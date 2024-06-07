#ifndef __HTTPSERVERCGI_H
#define __HTTPSERVERCGI_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
//#include "adc.h"



int init_Th_srv(void);
osThreadId_t get_id_Th_srv(void);
osMessageQueueId_t get_id_MsgQueue_srv(void);
osMessageQueueId_t get_id_MsgQueue_reg(void);
#endif

