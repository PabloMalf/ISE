#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_ADC.h"                  // ::Board Support:A/D Converter

#include "lcd.h"

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

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);

void app_main (void *arg);


/*----------------------------------------------------------------------------
  ADC handler
 *---------------------------------------------------------------------------*/
uint16_t AD_in (uint32_t ch) {
	int32_t val = 0;

  if (ch == 0) {
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
  return ((uint16_t)val);
}

/* Read digital inputs */
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    //IP address change, trigger LCD update 
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  LCD display handler
 *---------------------------------------------------------------------------*/
static void Display (void *arg) {
	LEDrun = false;
	
	static char buf[24];

	init(); //IMPORTANTE --> inicializar y resetear el LCD aqui y no en el main del micro o en el del servidor.
	LCD_reset();
	
	EscribeFraseL1(lcd_text[0]);
	EscribeFraseL2(lcd_text[1]);

  while(1) {
		osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);
		
		sprintf(buf, "%-20s", lcd_text[0]);
		sprintf(buf, "%-20s", lcd_text[1]);
		
		limpiardisplay();
		EscribeFraseL1(lcd_text[0]);		
		EscribeFraseL2(lcd_text[1]);
  }
}

/*----------------------------------------------------------------------------
  Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static void BlinkLed (void *arg) {
  const uint8_t led_val[6] = { 0x01,0x02,0x04,
															 0x08,0x04,0x02 };

  uint32_t cnt = 0U;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
void app_main (void *arg) {
	(void)arg;

	LED_Initialize();
	Buttons_Initialize();
	ADC_Initialize();
	
  netInitialize();
	
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}
