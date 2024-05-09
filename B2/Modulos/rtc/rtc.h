#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "cmsis_os2.h"  

#define AUTO_SYNC_TIME_S 180000U

int init_Th_rtc(void);

typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} mytime_t;

extern mytime_t g_time;

#endif /* __RTC_H */

