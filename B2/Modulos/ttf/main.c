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

static osThreadId_t id_Th_testWR_reg;
static int	init_Th_testWR_reg(void);
static void Th_testWR_reg(void *arg);

static osThreadId_t id_Th_testWR_usu;
static int	init_Th_testWR_usu(void);
static void Th_testWR_usu(void *arg);

static osThreadId_t id_Th_testRD_reg;
static int	init_Th_testRD_reg(void);
static void Th_testRD_reg(void *arg);

static osThreadId_t id_Th_testRD_usu;
static int	init_Th_testRD_usu(void);
static void Th_testRD_usu(void *arg);

static void Error_Handler(void);	
static void SystemClock_Config(void);

typedef enum{PERMITIDO, DENEGADO, DESCONOCIDO} tipo_acceso_t;
typedef enum{H, M, poco} sexo_t;

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
	char pin [4];
	uint8_t sNum[5];
	sexo_t sexo;
}INFO_PERSONA_T;

typedef struct{
	mytime_t fecha;
	tipo_acceso_t acceso;
	INFO_PERSONA_T persona;
}INFO_REGISTRO_T;

static void WR_Register(INFO_REGISTRO_T registro);
static void WR_Usuario(INFO_PERSONA_T usuario);


int main(void){
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();

#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

	//start Threads 
	init_Th_ttf();
	//init_Th_testWR_reg();
	//init_Th_testRD_reg();
	init_Th_testWR_usu();
	init_Th_testRD_usu();
	
	
  osKernelStart();
#endif
  while(1){}
}

int init_Th_testWR_reg(void){
	id_Th_testWR_reg = osThreadNew(Th_testWR_reg, NULL, NULL);
	if(id_Th_testWR_reg == NULL)
		return(-1);
	return(0);
}
int init_Th_testWR_usu(void){
	id_Th_testWR_usu = osThreadNew(Th_testWR_usu, NULL, NULL);
	if(id_Th_testWR_usu == NULL)
		return(-1);
	return(0);
}

int init_Th_testRD_reg(void){
	id_Th_testRD_reg = osThreadNew(Th_testRD_reg, NULL, NULL);
	if(id_Th_testRD_reg == NULL)
		return(-1);
	return(0);
}

int init_Th_testRD_usu(void){
	id_Th_testRD_usu = osThreadNew(Th_testRD_usu, NULL, NULL);
	if(id_Th_testRD_usu == NULL)
		return(-1);
	return(0);
}

static void WR_Register(INFO_REGISTRO_T registro){
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf_mosi;
	msg_ttf_mosi.cmd=WR;
	msg_ttf_mosi.fichero=REG;
	sprintf(msg_ttf_mosi.data,"%02d/%02d/%02d,%02d:%02d:%02d,%s,%02X %02X %02X %02X %02X,%d,\n",
	         registro.fecha.day,registro.fecha.month,registro.fecha.year,
	         registro.fecha.hour,registro.fecha.min,registro.fecha.sec,
	         registro.persona.nombre,
	         registro.persona.sNum[0],registro.persona.sNum[1],
	         registro.persona.sNum[2], registro.persona.sNum[3],registro.persona.sNum[4],
	         registro.acceso);
	
		
	osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf_mosi, NULL, osWaitForever);
	memset(msg_ttf_mosi.data, '\0', sizeof(msg_ttf_mosi.data));
}

static void WR_Usuario(INFO_PERSONA_T usuario){
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf_mosi;
	msg_ttf_mosi.cmd=WR;
	msg_ttf_mosi.fichero=USER;

	sprintf(msg_ttf_mosi.data,"%02X %02X %02X %02X %02X,%s,%s,%d,\n",
	         usuario.sNum[0],usuario.sNum[1],
	         usuario.sNum[2], usuario.sNum[3],usuario.sNum[4],
	         usuario.nombre, usuario.pin,
	         usuario.sexo);
	
	osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf_mosi, NULL, osWaitForever);
	memset(msg_ttf_mosi.data, '\0', sizeof(msg_ttf_mosi.data));
}

void Th_testWR_reg(void *arg){ 
	
	INFO_REGISTRO_T registro;
	
	registro.fecha.day=6;
	registro.fecha.month=6;
	registro.fecha.year=25;
	
	registro.fecha.hour=05;
	registro.fecha.min=55;
	registro.fecha.sec=54;
	
	registro.persona.sNum[0]= 0x01;
	registro.persona.sNum[1]= 0x02;
	registro.persona.sNum[2]= 0x03;
	registro.persona.sNum[3]= 0x04;
	registro.persona.sNum[4]= 0x05;
	
	registro.acceso=PERMITIDO;
  strcpy(registro.persona.nombre, "CARLA");
	
	while(1){
		printf("Write REG\n");
		
		WR_Register(registro);
		osDelay(1000000);
	
	
	}
}

void Th_testWR_usu(void *arg){ 
	INFO_PERSONA_T  usuario;
	
	usuario.sNum[0]= 0x06;
	usuario.sNum[1]= 0x07;
	usuario.sNum[2]= 0x08;
	usuario.sNum[3]= 0x09;
	usuario.sNum[4]= 0x10;
	usuario.sexo=H;
	strcpy(usuario.nombre, "Franote");
	strcpy(usuario.pin, "1234");
	
	while(1){
		printf("Write USU\n");
		
		WR_Usuario(usuario);
		osDelay(1000000);
	
	}
}

void Th_testRD_reg(void *arg){ 
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso_main;
	int i,j=0;
	while(1){
		osDelay(2000);
		msg_ttf.cmd=RD;
		msg_ttf.fichero=REG;
		osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever);
		
		osMessageQueueGet(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso_main, NULL, osWaitForever);
		 for (j=0; j<50;j++) {
			 for(i=0; i<5;i++)
            printf("Registro[%d]campo%d : %s\n", j,i, msg_ttf_miso_main.datos[j][i].valor);
        }
		osDelay(20000);
	}
}

void Th_testRD_usu(void *arg){ 
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	MSGQUEUE_OBJ_TTF_MISO msg_ttf_miso_main;
	int i,j=0;
	while(1){
		osDelay(2000);
		msg_ttf.cmd=RD;
		msg_ttf.fichero=USER;
		osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever);
		
		osMessageQueueGet(get_id_MsgQueue_ttf_miso(), &msg_ttf_miso_main, NULL, osWaitForever);
		 for (j=0; j<50;j++) {
			 for(i=0; i<4;i++)
            printf("Usarios[%d]campo%d : %s\n", j,i, msg_ttf_miso_main.datos[j][i].valor);
        }
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
