#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "rtc.h"
                     // CMSIS RTOS header file
#include <time.h>
#include "rl_net_lib.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

mytime_t g_time;

RTC_HandleTypeDef RtcHandle;
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

static uint32_t exec;  
osTimerId_t tim_id_3min;

struct tm ts;
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

static osThreadId_t id_Th_rtc;
static void Th_rtc(void *arg);
int init_Th_rtc(void);

// Functions
void init_rtc(void);
void init_SNTP(void);
void RTC_CalendarConfig(uint8_t hour, uint8_t min, uint8_t sec,  uint8_t day, uint8_t month,  uint8_t year);
void RTC_Show(void);
void Init_timers (void);

static void Timer_Callback_3min (void const *arg) {
	 init_SNTP(); // Cada 3 min se llama al servidor SNTP
}

void Init_timers (void) {
	exec = 1U;
	tim_id_3min = osTimerNew((osTimerFunc_t)&Timer_Callback_3min, osTimerPeriodic, &exec, NULL);

}

int init_Th_rtc(void){
	id_Th_rtc = osThreadNew(Th_rtc, NULL, NULL);
	if(id_Th_rtc == NULL)
		return(-1);
	return(0);
}

void init_rtc(void){
  /*##-1- Configure the rtc peripheral #######################################*/
  /* Configure rtc prescaler and rtc data registers */
  /* rtc configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
	
  HAL_RTC_Init(&RtcHandle);
  init_SNTP();
	
}


/***Configuracion********/
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();

  /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
  HAL_PWR_DisableBkUpAccess();
  __HAL_RCC_PWR_CLK_DISABLE();
  
}
void RTC_Show()
{
  HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
	
	g_time.year = 2000 + sdatestructure.Year;
	g_time.month = sdatestructure.Month;
	g_time.day = sdatestructure.Date;
	g_time.hour = stimestructure.Hours;
	g_time.min= stimestructure.Minutes;
	g_time.sec = stimestructure.Seconds;

}

/***SNTP***/
void init_SNTP (void) {
  netStatus status = netSNTPc_GetTime (NULL, time_callback);
}

static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  if (seconds != 0) {
   ts = *localtime(&seconds);
   sdatestructure.Year = ts.tm_year - 100;
   sdatestructure.Month = ts.tm_mon + 1;
   sdatestructure.Date = ts.tm_mday;
   sdatestructure.WeekDay = ts.tm_wday;
  
   HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

   stimestructure.Hours = ts.tm_hour + 2 ;
   stimestructure.Minutes = ts.tm_min;
   stimestructure.Seconds = ts.tm_sec;
   stimestructure.TimeFormat = RTC_HOURFORMAT_24;
   stimestructure.DayLightSaving = ts.tm_isdst ;
   stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
   
	 HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	
	   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
  }
	
	else{
	sdatestructure.Year = 4 ;
  sdatestructure.Month = RTC_MONTH_MAY; //RTC_MONTH_FEBRUARY
  sdatestructure.Date = 13;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD);


  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 3;
  stimestructure.Minutes = 30;
  stimestructure.Seconds = 3;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD);


  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
		
	}
}


static void Th_rtc(void *argument){
  netInitialize();
	init_rtc();
	Init_timers();
	osTimerStart(tim_id_3min, AUTO_SYNC_TIME_S);
	while(1){
	  RTC_Show();
    osDelay (1000);
	  osThreadYield();
	}
}
