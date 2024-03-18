#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#include "lcd.h"
#include "adc.h"


// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

extern osThreadId_t TID_Led; //KKK prob no need

bool LEDrun; //KKK prob no need
ADC_HandleTypeDef hadc;

// Thread IDs 
osThreadId_t TID_Led; //KKK prob no need

// Thread declarations 
static void BlinkLed (void *arg);

void app_main (void *arg);


// Read digital inputs 
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

// IP address change notification
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    //osThreadFlagsSet (TID_Display, 0x01);    // IP address change, trigger LCD update
  }
}


//-----------------------------------------------------
//Thread 'BlinkLed': Blink the LEDs on an eval board
//-----------------------------------------------------
static void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x01,0x02,0x04,
															 0x08,0x04,0x02 };
  uint32_t cnt = 0U;
															 
  LEDrun = true;
  while(1) {
    // Every 100 ms 
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

void app_main (void *arg) {
  LED_Initialize();
  Buttons_Initialize();
	//myADC_Init(&hadc);

	LCD_init();
	
  //netInitialize();

  //TID_Led     = osThreadNew (BlinkLed, NULL, NULL); //KKK Prob no need

	char L1 [32], L2 [32];
	while(true){
		sprintf(L1, "1");
		sprintf(L2, "2");
		write_LCD(L1, L2);
		printf("a");
		osDelay(1000);
		sprintf(L1, "a");
		sprintf(L2, "B");
		write_LCD(L1, L2);
		printf("a");
		osDelay(1000);
	}
	
  osThreadExit();
}
