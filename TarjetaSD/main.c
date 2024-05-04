#include "main.h"
#include <stdio.h>

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

static osThreadId_t id_Th_testWR;
static int	init_Th_testWR(void);
static void Th_testWR(void *arg);

static osThreadId_t id_Th_testRD;
static int	init_Th_testRD(void);
static void Th_testRD(void *arg);

static void Error_Handler(void);	
static void SystemClock_Config(void);

int main(void){
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();

#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

	//start Threads 
	init_Th_ttf();
	//init_Th_testWR();
	init_Th_testRD();
	//
	
  osKernelStart();
#endif
  while(1){}
}

int init_Th_testWR(void){
	id_Th_testWR = osThreadNew(Th_testWR, NULL, NULL);
	if(id_Th_testWR == NULL)
		return(-1);
	return(0);
}

void Th_testWR(void *arg){ 
	MSGQUEUE_OBJ_TTF msg_ttf;
	while(1){
		msg_ttf.state=WR;
		sprintf(msg_ttf.name, "ISE 2024");
		osMessageQueuePut(get_id_MsgQueue_ttf(), &msg_ttf, 0, 0);
		osDelay(1000);
	}
}

int init_Th_testRD(void){
	id_Th_testRD = osThreadNew(Th_testRD, NULL, NULL);
	if(id_Th_testRD == NULL)
		return(-1);
	return(0);
}

void Th_testRD(void *arg){ 
	MSGQUEUE_OBJ_TTF msg_ttf;
	while(1){
		msg_ttf.state=RD;
		osMessageQueuePut(get_id_MsgQueue_ttf(), &msg_ttf, 0, 0);
		osDelay(2000);
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
