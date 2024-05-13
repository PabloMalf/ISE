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

static void Error_Handler(void);	
static void SystemClock_Config(void);
static void StandbyMode_Measure(void);

typedef enum{MAIN_PSU, BATTERY_PSU} ali_state_t;
uint8_t var=0;

static void StandbyMode_Measure(void)
{
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* Allow access to Backup */
  HAL_PWR_EnableBkUpAccess();

  /* Reset RTC Domain */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();
  
  /* Disable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  
  /* Clear all related wakeup flags */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Re-enable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();  
}

static void GPIO_Init(void){
	static GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

void EXTI0_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==GPIO_PIN_0){
		var=1;	
	}
	
}


void 	Th_principal(void *arg);

int main(void){
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();
	
	/* Configure LED1 and LED2 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
	BSP_LED_Init(LED3);

/* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Check and handle if the system was resumed from Standby mode */ 
  
  /* Ethernet PHY must be in low power mode in order to have the lowest current consumption */
  //ETH_PhyEnterPowerDownMode();

#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

	osThreadNew(Th_principal, NULL, NULL);
	
  osKernelStart();
#endif
    while (1)
    {
    }
}


void mode_main_psu(void){
	while(1){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) return; //go sleep
		BSP_LED_Toggle(LED3);
		osDelay(1000);
		
		if(var==1){
		var=0;
			 BSP_LED_Toggle(LED2);
			osDelay(1000);
			BSP_LED_Toggle(LED2);	
	  }
  }
}

void Th_principal(void*arg){
	ali_state_t ali_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) ? MAIN_PSU : BATTERY_PSU;
	GPIO_Init();
	switch(ali_state){
		case MAIN_PSU:
			mode_main_psu();	
		break;
		
		case BATTERY_PSU:
			BSP_LED_Toggle(LED2);
			osDelay(1000);
			BSP_LED_Toggle(LED2);
		break;
	}
	 StandbyMode_Measure();
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
