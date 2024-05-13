#include "main.h"
#include <stdio.h>
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

static osThreadId_t id_Th_testWR;
static int	init_Th_testWR(void);
static void Th_testWR(void *arg);

static osThreadId_t id_Th_testRD;
static int	init_Th_testRD(void);
static void Th_testRD(void *arg);

static void Error_Handler(void);	
static void SystemClock_Config(void);

typedef enum{PERMITIDO, DENEGADO, DESCONOCIDO} tipo_acceso_t;

typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} mytime_t;

typedef struct{
	char nombre [15];
	uint8_t sNum[5];
}INFO_PERSONA_T;

typedef struct{
	mytime_t fecha;
	tipo_acceso_t acceso;
	INFO_PERSONA_T persona;
}INFO_REGISTRO_T;

static void WR_Register(INFO_REGISTRO_T registro);


int main(void){
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();

#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

	//start Threads 
	init_Th_ttf();
	init_Th_testWR();
	//init_Th_testRD();
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

static void WR_Register(INFO_REGISTRO_T registro){
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf_mosi;
	msg_ttf_mosi.cmd=WR;
	sprintf(msg_ttf_mosi.data[0], "%02d/%02d/%02d",registro.fecha.day,registro.fecha.month,registro.fecha.year);
	sprintf(msg_ttf_mosi.data[1], "%02d:%02d:%02d",registro.fecha.hour,registro.fecha.min,registro.fecha.sec);
	sprintf(msg_ttf_mosi.data[2], "%s",registro.persona.nombre);
	sprintf(msg_ttf_mosi.data[3], "%02X %02X %02X %02X %02X",registro.persona.sNum[0], registro.persona.sNum[1],
		                                                                   registro.persona.sNum[2], registro.persona.sNum[3],
		                                                                   registro.persona.sNum[4]);
	sprintf(msg_ttf_mosi.data[4], "%d",registro.acceso);
		
	osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf_mosi, NULL, osWaitForever);
}

void Th_testWR(void *arg){ 
	
	INFO_REGISTRO_T registro;
	registro.fecha.day=8;
	registro.fecha.month=8;
	registro.fecha.year=22;
	
	registro.fecha.hour=12;
	registro.fecha.min=12;
	registro.fecha.sec=12;
	
	registro.persona.sNum[0]= 0x24;
	registro.persona.sNum[1]= 0xD6;
	registro.persona.sNum[2]= 0x26;
	registro.persona.sNum[3]= 0x27;
	registro.persona.sNum[4]= 0x66;
	
	registro.acceso= PERMITIDO;
	
  strcpy(registro.persona.nombre, "Francisco");
	
	while(1){
		osDelay(5000);
		WR_Register(registro);
	
	}
}

int init_Th_testRD(void){
	id_Th_testRD = osThreadNew(Th_testRD, NULL, NULL);
	if(id_Th_testRD == NULL)
		return(-1);
	return(0);
}

void Th_testRD(void *arg){ 
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso_main;
	while(1){
		
		msg_ttf.cmd=RD;
		osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever);
		
		osMessageQueueGet(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso_main, NULL, osWaitForever);
		osDelay(20000);
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
