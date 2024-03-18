#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"
#include "stdint.h"

#ifndef __RTC_H
#define __RTC_H

#include "stdio.h"
#include "string.h"
#include "Parpadeo.h"

void RTC_Init(void);
void Get_Time_RTC_Binary(void);
void Set_Time_RTC(uint8_t hour, uint8_t minute, uint8_t second);
void Get_Date_RTC(void);
void Set_RTC_Date(uint8_t year, uint8_t month, uint8_t week, uint8_t date);
void Set_Alarm(uint8_t hour, uint8_t minute, uint8_t second);

void Display_Date_Time(void);
#endif
