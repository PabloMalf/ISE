//#include "stm32f4xx_hal.h"
//#include <stdio.h>
//#include <time.h>
//#include "rl_net_lib.h"

//extern RTC_DateTypeDef sdatestructure;
//extern RTC_TimeTypeDef stimestructure;
////extern RTC_DateTypeDef sdatestructure;
////extern RTC_TimeTypeDef stimestructure;
////RTC_HandleTypeDef SNTPHandle;
//extern RTC_HandleTypeDef RtcHandle;

//struct tm ts;
//    
//static void time_callback (uint32_t seconds, uint32_t seconds_fraction);
// 

//void init_SNTP (void) {
// netSNTPc_GetTime (NULL, time_callback);
//}
// 
//static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
//  if (seconds != 0) {
//   ts = *localtime(&seconds);
//   sdatestructure.Year = ts.tm_year - 100;
//   sdatestructure.Month = ts.tm_mon + 1;
//   sdatestructure.Date = ts.tm_mday;
//   sdatestructure.WeekDay = ts.tm_wday;
//  
//   HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

//   stimestructure.Hours = ts.tm_hour + 1 ;
//   stimestructure.Minutes = ts.tm_min;
//   stimestructure.Seconds = ts.tm_sec;
//   stimestructure.TimeFormat = RTC_HOURFORMAT_24;
//   stimestructure.DayLightSaving = ts.tm_isdst ;
//   stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
//   
//	 HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
//	
//	   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
//  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
//  }
//}









