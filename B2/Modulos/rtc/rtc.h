#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h" 

#define AUTO_SYNC_TIME_S 180000U

typedef struct{
	uint8_t seg;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} mytime_t;

extern mytime_t g_time;

int init_Th_rtc(void);

#endif

