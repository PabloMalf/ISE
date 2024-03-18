#include <stdio.h>

#include "main.h"

#include "rl_net.h"										 // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"							// Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"									// ::Board Support:LED
#include "Board_Buttons.h"							// ::Board Support:Buttons

#include "lcd.h"
#include "adc.h"


// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
	.stack_mem	= &app_main_stk[0],
	.stack_size = sizeof(app_main_stk)
};

osThreadId_t TID_Led;
extern osThreadId_t TID_Led;

void Th_LED (void *arg);
void app_main (void *arg);

static void print_IPV4 (void);

extern uint8_t get_button	(void);
extern void	netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

ADC_HandleTypeDef hadc;


uint8_t get_button (void) {
	return ((uint8_t)Buttons_GetState());
}

void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {
	if (option == NET_DHCP_OPTION_IP_ADDRESS) {
		print_IPV4();
	}
}

static void print_IPV4 (void){
	char ip[40];
	char L1[SIZE_LCD_LANE];
	uint8_t addr[NET_ADDR_IP6_LEN];

	netIF_GetOption (NET_IF_CLASS_ETH, netIF_OptionIP4_Address, addr, sizeof(addr));
	netIP_ntoa(NET_ADDR_IP4, addr, ip, sizeof(ip));
	sprintf(L1, " IPv4: %-16s", ip);
	write_LCD_L1(L1);
}



//-----------------------------------------------------
// Main Thread 'main': Run Network
//-----------------------------------------------------
void app_main (void *arg) {
	LED_Initialize();
	Buttons_Initialize();
	myADC_Init(&hadc);

	LCD_init();
	
	netInitialize();
	
	print_IPV4();
	printf("\naaaa\n");
	
	TID_Led     = osThreadNew (Th_LED, NULL, NULL);

	osThreadExit();
}
