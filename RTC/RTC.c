#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "RTC.h"
                     // CMSIS RTOS header file
#include <time.h>
#include "rl_net_lib.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

//Timer
static uint32_t exec;  
osTimerId_t tim_id_3min;

// SNTP
struct tm ts;
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

// Thread Keypad
static osThreadId_t id_Th_RTC;
static void Th_RTC(void *arg);
int init_Th_RTC(void);

// Queue Keypad
static osMessageQueueId_t id_MsgQueue_RTC;
static int Init_MsgQueue_RTC(void);

// Functions
void init_RTC(void);
void init_SNTP(void);
void RTC_CalendarConfig(uint8_t hour, uint8_t min, uint8_t sec,  uint8_t day, uint8_t month,  uint8_t year);
void RTC_Show(void);
void Init_timers (void);

static int Init_MsgQueue_RTC(){
	id_MsgQueue_RTC = osMessageQueueNew(MSGQUEUE_OBJECTS_RTC, sizeof(MSGQUEUE_OBJ_RTC), NULL);
	if(id_MsgQueue_RTC == NULL)
		return (-1); 
	return(0);
}

osMessageQueueId_t get_id_MsgQueue_RTC(void){
	return id_MsgQueue_RTC;
}

static void Timer_Callback_3min (void const *arg) {
	 init_SNTP(); // Cada 3 min se llama al servidor SNTP
}

void Init_timers (void) {
	exec = 1U;
	tim_id_3min = osTimerNew((osTimerFunc_t)&Timer_Callback_3min, osTimerPeriodic, &exec, NULL);

}

int init_Th_RTC(void){
	id_Th_RTC = osThreadNew(Th_RTC, NULL, NULL);
	if(id_Th_RTC == NULL)
		return(-1);
	return(Init_MsgQueue_RTC());
}

//Configuracion
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

void init_RTC(void){
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
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

void RTC_Show()
{
  MSGQUEUE_OBJ_RTC RTC_data;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
	
  /* Display time Format : hh:mm:ss */
  sprintf(RTC_data.hour, "%02d:%02d:%02d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
	
  /* Display date Format : mm-dd-yy */
  sprintf(RTC_data.date, "%02d-%02d-%02d",sdatestructure.Date, sdatestructure.Month, 2000 + sdatestructure.Year);
	osMessageQueuePut(id_MsgQueue_RTC, &RTC_data, 0, 0);
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
}


static void Th_RTC(void *argument){
  netInitialize();
	init_RTC();
	Init_timers();
	osTimerStart(tim_id_3min, 180000U);
	while(1){
	  RTC_Show();
    osDelay (1000);
	  osThreadYield();
	}
}
