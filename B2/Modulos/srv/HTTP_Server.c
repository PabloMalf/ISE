//#include "cmsis_os2.h"                          // CMSIS RTOS header file
//#include <stdio.h>
//#include "main.h"
//#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
//#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

//#include "HTTP_Server.h"

//#include <string.h>

////// Main stack size must be multiple of 8 Bytes
////#define APP_MAIN_STK_SZ (1024U)
////uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
////const osThreadAttr_t app_main_attr = {
////  .stack_mem  = &app_main_stk[0],
////  .stack_size = sizeof(app_main_stk)
////};

//static osThreadId_t id_Th_srv;
//static void Th_srv(void *arg);

//int init_Th_srv(void){
//	id_Th_srv = osThreadNew(Th_srv, NULL, NULL);
//	if(id_Th_srv == NULL)
//		return(-1);
//	return(0);
//}

//__NO_RETURN void Th_srv (void *arg) {
//  (void)arg;
//  
//  netInitialize();
//  osThreadExit();
//}


