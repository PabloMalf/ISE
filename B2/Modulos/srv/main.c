#include "main.h"
#include <stdio.h>
#include "HTTP_Server_CGI.h"
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

#define REGISTROS 15
#define CAMPOS_REG 5
#define CAMPOS_USU 4

static osThreadId_t id_Th_test;

static int	init_Th_test(void);
static void Th_test(void *arg);

static void Error_Handler(void);	
static void SystemClock_Config(void);

typedef struct{
  char valor[20];
} string;

typedef struct{
  string datos[REGISTROS][CAMPOS_REG];
	uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (pila)
} MSGQUEUE_OBJ_SRV;

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

sprintf(msg_srv.datos[0][0].valor, "11:11:11");
sprintf(msg_srv.datos[0][1].valor, "11/11/2011");
sprintf(msg_srv.datos[0][2].valor, "Andrea");
sprintf(msg_srv.datos[0][3].valor, "11 11 11 11");
sprintf(msg_srv.datos[0][4].valor, "denegado");
	
sprintf(msg_srv.datos[1][0].valor, "22:22:22");
sprintf(msg_srv.datos[1][1].valor, "22/02/2022");
sprintf(msg_srv.datos[1][2].valor, "Juan");
sprintf(msg_srv.datos[1][3].valor, "22 22 22 22");
sprintf(msg_srv.datos[1][4].valor, "desconocido");

sprintf(msg_srv.datos[2][0].valor, "33:23:23");
sprintf(msg_srv.datos[2][1].valor, "31/03/2023");
sprintf(msg_srv.datos[2][2].valor, "Eustaquio");
sprintf(msg_srv.datos[2][3].valor, "33 33 33 33");
sprintf(msg_srv.datos[2][4].valor, "admitido");
	
sprintf(msg_srv.datos[3][0].valor, "44:44:00");
sprintf(msg_srv.datos[3][1].valor, "24/04/2024");
sprintf(msg_srv.datos[3][2].valor, "Cojonciado");
sprintf(msg_srv.datos[3][3].valor, "44 44 44 44");
sprintf(msg_srv.datos[3][4].valor, "admitido");

  
  msg_srv.standBy=0;


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
