/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "RTC.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Flags.h"
#include <time.h>
#include "rl_net_lib.h"

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
RTC_AlarmTypeDef  alarmRtc;
RTC_DateTypeDef sdatestructure;
RTC_TimeTypeDef stimestructure;

 /*** Thread Alarma***/
osThreadId_t tid_ThAlarm;                        // thread id
void ThAlarm (void *argument);                   // thread function

/*** SNTP ***/
struct tm ts;
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

/**** Timers ****/
static uint32_t exec;                             // argument for the timer call back function
osTimerId_t tim_id_1s;
osTimerId_t tim_id_3min;
osTimerId_t tim_id_500ms;


int segundos=0; //Variable control de 5 segundos de parpadeo tras la alarma

/**** Pulsador ****/
static GPIO_InitTypeDef  GPIO_InitStruct;
void Pulsador_config(void){
  __HAL_RCC_GPIOC_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  
  /*Configure GPIO pin : PC13 - USER BUTTON */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
/**************/

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

  //RTC_CalendarConfig();
  init_SNTP();
	RTC_AlarmConfig();
  Pulsador_config();
	
	Init_timers();
	osTimerStart(tim_id_3min, 180000U);
	tid_ThAlarm = osThreadNew(ThAlarm, NULL, NULL); //Se crea el hilo de la Alarma
  

}




/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
 void RTC_CalendarConfig(uint8_t hour, uint8_t min, uint8_t sec,  uint8_t day, uint8_t month,  uint8_t year)
{

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = year;
  sdatestructure.Month = month; //RTC_MONTH_FEBRUARY
  sdatestructure.Date = day;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD);


  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = hour;
  stimestructure.Minutes = min;
  stimestructure.Seconds = sec;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD);


  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);
	
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%02d:%02d:%02d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);

  /* Display date Format : mm-dd-yy */
   sprintf((char *)showdate, "%02d-%02d-%02d",sdatestructure.Date, sdatestructure.Month, 2000 + sdatestructure.Year);
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

   stimestructure.Hours = ts.tm_hour + 1 ;
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


/***ALARMA*****/
void RTC_AlarmConfig(void)
{
  alarmRtc.AlarmTime.Hours =  stimestructure.Hours;
  alarmRtc.AlarmTime.Minutes = stimestructure.Minutes;
  alarmRtc.AlarmTime.Seconds = 0;
  alarmRtc.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  alarmRtc.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  alarmRtc.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
  alarmRtc.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarmRtc.AlarmDateWeekDay = 0x1;
  alarmRtc.Alarm = RTC_ALARM_A;
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	// Enable the alarm interrupt
	HAL_RTC_SetAlarm_IT(&RtcHandle, &alarmRtc, RTC_FORMAT_BIN);
	
	// Unmask the RTC Alarm A interrupt
	CLEAR_BIT(RtcHandle.Instance->CR, RTC_CR_ALRAIE);
	
}

//Pulsador Azul
	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if(GPIO_Pin==GPIO_PIN_13)
    RTC_CalendarConfig(0,0,0,1,1,0); //hour,min,sec day,month,year
  }

static void Timer_Callback_1s (void const *arg) {
  segundos++;
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
	if (segundos==6){
		segundos=0;
	  //osThreadFlagsSet (tid_ThAlarm,Flag_FinParpadeo);
		osTimerStop(tim_id_1s);
	}
}

static void Timer_Callback_3min (void const *arg) {
	 init_SNTP(); // Cada 3 min se llama al servidor SNTP
	 osTimerStart(tim_id_500ms, 500U);
}

static void Timer_Callback_500ms (void const *arg) {
	segundos++;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==RESET){
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
  }
  else if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==SET)
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
  if (segundos==4){
		segundos=0;
		osTimerStop(tim_id_500ms);
  }
}

void Init_timers (void) {
	exec = 1U;
	tim_id_1s = osTimerNew((osTimerFunc_t)&Timer_Callback_1s, osTimerPeriodic, &exec, NULL);
	tim_id_3min = osTimerNew((osTimerFunc_t)&Timer_Callback_3min, osTimerPeriodic, &exec, NULL);
	tim_id_500ms = osTimerNew((osTimerFunc_t)&Timer_Callback_500ms, osTimerPeriodic, &exec, NULL);
}

 // Thread Alarma
void ThAlarm (void *argument) {
   //Init_timers();  //Se inicializan los timers
	 //osTimerStart(tim_id_3min, 180000U); // Se arranca el timer de sincronización
  while (1) {
    ; // Insert thread code here...
		osThreadFlagsWait(Flag_Alarm, osFlagsWaitAny, osWaitForever);
		osTimerStart(tim_id_1s, 1000U);			
	  osThreadYield(); // suspend thread  
  }

}





