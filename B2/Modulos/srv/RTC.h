
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */



/* Functions ------------------------------------------------------- */
void Init_timers (void);
void init_RTC(void);
void init_SNTP(void);
void RTC_CalendarConfig(uint8_t hour, uint8_t min, uint8_t sec,  uint8_t day, uint8_t month,  uint8_t year);
void RTC_AlarmConfig(void);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);

#endif /* __RTC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
