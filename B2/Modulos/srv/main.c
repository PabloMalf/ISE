#include "main.h"
#include <stdio.h>
#include "HTTP_Server.h"
#include <string.h>

#ifdef _RTE_
#include "RTE_Components.h"
#endif
#ifdef RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
uint32_t HAL_GetTick(void){
  static uint32_t ticks = 0U;
         uint32_t i;

  if(osKernelGetState() == osKernelRunning){
    return ((uint32_t)osKernelGetTickCount());
  }

  for(i = (SystemCoreClock >> 14U); i > 0U; i--){
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ticks++;
}
#endif

static osThreadId_t id_Th_test;

static int	init_Th_test(void);
static void Th_test(void *arg);

static char adtos[50][20];

static void Error_Handler(void);	
static void SystemClock_Config(void);

int main(void){
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();

#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

	//start Threads 
	init_Th_srv();
	init_Th_test();
	//
	
  osKernelStart();
#endif
  while(1){}
}

int init_Th_test(void){
	id_Th_test = osThreadNew(Th_test, NULL, NULL);
	if(id_Th_test == NULL)
		return(-1);
	return(0);
}

void Th_test(void *arg){ //Test del modulo
  MSGQUEUE_OBJ_SRV msg_srv;
  int j;
  
  sprintf(adtos[0],"12:12:12");
  sprintf(adtos[1],"12/12/2023");
  sprintf(adtos[2],"Claudia");
  sprintf(adtos[3],"111111111");
  sprintf(adtos[4],"permitido");
  
  sprintf(adtos[5],"44:44:44");
  sprintf(adtos[6],"44/44/2024");
  sprintf(adtos[7],"Roberto");
  sprintf(adtos[8],"222222222");
  sprintf(adtos[9],"desconocido");
  
  sprintf(adtos[10],"44:44:44");
  sprintf(adtos[11],"44/44/2024");
  sprintf(adtos[12],"Andrea");
  sprintf(adtos[13],"333333333");
  sprintf(adtos[14],"denegado");
  
  sprintf(adtos[15],"44:44:44");
  sprintf(adtos[16],"44/44/2024");
  sprintf(adtos[17],"Andrea");
  sprintf(adtos[18],"333333333");
  sprintf(adtos[19],"denegado");
  
  sprintf(adtos[20],"44:44:44");
  sprintf(adtos[21],"44/44/2024");
  sprintf(adtos[22],"Andrea");
  sprintf(adtos[23],"333333333");
  sprintf(adtos[24],"denegado");
  
  sprintf(adtos[25],"44:44:44");
  sprintf(adtos[26],"44/44/2024");
  sprintf(adtos[27],"Andrea");
  sprintf(adtos[28],"333333333");
  sprintf(adtos[29],"denegado");
  
  sprintf(adtos[30],"44:44:44");
  sprintf(adtos[31],"44/44/2024");
  sprintf(adtos[32],"Andrea");
  sprintf(adtos[33],"333333333");
  sprintf(adtos[34],"denegado");
  
  
  for (j = 0; j < 50; j++) {
     strcpy(msg_srv.adtos[j], adtos[j]);
  }
  
  msg_srv.standBy=1;


	while(1){
   osMessageQueuePut(get_id_MsgQueue_srv(), &msg_srv, NULL, osWaitForever);
	}
}

static void SystemClock_Config(void){
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }
	
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
    Error_Handler();
  }
  if (HAL_GetREVID() == 0x1001){
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

static void Error_Handler(void){ while(1){} }

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){ while(1){} }
#endif 
