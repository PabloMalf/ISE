#include "principal.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

//kkk change ip Net_Conifg_ETH_0
//kkk change buzz pin 7 

#define KEY_TIMEOUT_MS 4000U

typedef enum{BATTERY_PSU, MAIN_PSU} ali_state_t;

typedef enum{P_OFF, P_START, P_KEY, P_KEY_W8, P_DENEGADO_1, P_ERROR} pantallas_t;

typedef enum{R_NFC, R_KEY, R_END, R_EXIT} reg_state_t;

typedef enum{PERMITIDO, DENEGADO, DESCONOCIDO} tipo_acceso_t;

typedef enum{H, M, poco} sexo_t;


typedef struct{
	mytime_t time;
	MSGQUEUE_OBJ_LCD lcd;
	pantallas_t pantallas;
	uint8_t sNum[5];
	sexo_t sexo;
	char arg [2][21];
} MSGQUEUE_OBJ_GESTOR;


typedef struct{
	char Nombre [15];
	char pin [4];
	uint8_t sNum [5];
	sexo_t sexo;
}INFO_PERSONA_T;


typedef struct{
	mytime_t fecha;
	tipo_acceso_t acceso;
	INFO_PERSONA_T persona;
}INFO_REGISTRO_T;


typedef struct{
	MSGQUEUE_OBJ_BUZ buz;
	//MSGQUEUE_OBJ_CAM cam;
	MSGQUEUE_OBJ_KEY key;
	MSGQUEUE_OBJ_NFC nfc;
	MSGQUEUE_OBJ_RGB rgb;
	MSGQUEUE_OBJ_TTF_MOSI ttf_mosi;
	MSGQUEUE_OBJ_TTF_MISO ttf_miso;
}msg_t;


typedef struct{
	ali_state_t ali_state;
	char reg_error_msg [21];
	msg_t msg;
}data_t; 


const INFO_PERSONA_T personas_autorizadas [] = {
	{.Nombre = "Admin",		.sexo = H, .pin = "1234", .sNum = {0x83, 0x6a, 0x79, 0xfa, 0x6a}},
	{.Nombre = "Claudia",	.sexo = M, .pin = "2002", .sNum = {0x33, 0x8a, 0xcc, 0xe4, 0x91}},
	{.Nombre = "Manuel",	.sexo = H, .pin = "4389", .sNum = {0xe3, 0x82, 0xd9, 0xe4, 0x5c}},
	{.Nombre = "María",		.sexo = M, .pin = "7269", .sNum = {0x23, 0xd0, 0x0c, 0xe5, 0x1a}}
}; 
	

extern mytime_t g_time;
static osThreadId_t id_Th_principal;
static osThreadId_t id_Th_gestor;
static osMessageQueueId_t id_MsgQueue_gestor;


int init_Th_principal(void);

static void Th_principal(void *arg);
static void Th_gestor(void *arg);



static int time_updated(MSGQUEUE_OBJ_GESTOR* g);
	


int init_Th_principal(void){
	const osThreadAttr_t attr = {.stack_size = 2048};
	id_Th_principal = osThreadNew(Th_principal, NULL, &attr);
	if(id_Th_principal == NULL)
		return(-1);
	
	int buz = init_Th_buz();
	//int cam = init_Th_cam();
	int key = init_Th_key();
	int lcd = init_Th_lcd();
	int nfc = init_Th_nfc();
	int rgb = init_Th_rgb();
	int rtc = init_Th_rtc();
	int srv = init_Th_buz();//
	int ttf = init_Th_buz();//
	
	return(0);
}


static void init_pin_ali(void){
	GPIO_InitTypeDef sgpio = {0};

	//PIN_ALI_STATE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &sgpio);

	//PIN_IRQ MOV SOLO, NO PENSADO WKUP KKK
	__HAL_RCC_GPIOA_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_IT_FALLING;
	sgpio.Pull = GPIO_PULLDOWN;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &sgpio);
}


static void mode_main_psu(data_t* d){ //kkk
	
	//init SV
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	
	while(1){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) return; //Go to sleep
		//kkk
	}
}


static char* centrar(const char cadena[]) {
	static char exit[21];
	int longitud = strlen(cadena);
	int espacios = (20 - longitud) / 2;
	int i;
	
	for(i = 0; i < 20; i++){
		if(i < espacios) exit[i] = ' ';
		else if(i < espacios+longitud) exit[i] = cadena[i-espacios];
		else exit[i] = ' ';
	}
	
	return &(exit[0]);
}
				

static int get_persona(const uint8_t sNum [5]){
	int i;
	for(i = 0; i < sizeof(personas_autorizadas) / sizeof(personas_autorizadas[0]); i++){
		if(memcmp(sNum, personas_autorizadas[i].sNum, sizeof(personas_autorizadas[i].sNum)) == 0)
			return i;
	}
	return -1;
}


static void my_strcat (char *str, char c){
	for (;*str;str++); // note the terminating semicolon here. 
	*str++ = c; 
	*str++ = 0;
}


static void registro_acceso(void){
	MSGQUEUE_OBJ_GESTOR msg_gestor;
	MSGQUEUE_OBJ_LCD msg_lcd;
	MSGQUEUE_OBJ_NFC msg_nfc;
	MSGQUEUE_OBJ_KEY msg_key;
	MSGQUEUE_OBJ_BUZ msg_buz = {750, 200, 5};
	INFO_REGISTRO_T info;
	
	char p [4][4] = {"*...", "**..", "***.", "****"};
	char pin [4] = "";
	int aux;
	reg_state_t reg_state = R_NFC;
	
	msg_gestor.pantallas = P_START;
	osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
	
	while(reg_state != R_EXIT){
		switch(reg_state){
			case R_NFC:
				osThreadFlagsSet(get_id_Th_nfc(), NFC_FLAG_ON);
				if(osOK != osMessageQueueGet(get_id_MsgQueue_nfc(), &(msg_nfc), 0U, NFC_TIMEOUT_MS)){
					info.acceso = DESCONOCIDO;
					msg_gestor.pantallas = P_ERROR;
					sprintf(msg_gestor.arg[0], "  REGISTRADO COMO:  ");
					sprintf(msg_gestor.arg[1], "    DESCONOCIDO     ");
					osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
					reg_state = R_EXIT;
				}
				else{
					aux = get_persona(msg_nfc.sNum);
					if(aux != -1){ // Acceso autorizado
						info.persona = personas_autorizadas[aux];
						msg_gestor.sexo = info.persona.sexo;
						msg_gestor.pantallas = P_KEY;
						strcpy(msg_gestor.arg[0], centrar(info.persona.Nombre));
						sprintf(msg_gestor.arg[1], "....");
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
						reg_state = R_KEY;
					}
					else{ // Acceso denegado, regsitrado como desconocido
						info.acceso = DESCONOCIDO;
						msg_gestor.pantallas = P_DENEGADO_1;
						memcpy(msg_gestor.sNum, msg_nfc.sNum, sizeof(msg_gestor.sNum));
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
						reg_state = R_EXIT;
					}
				}
			break;
			
			case R_KEY:
				strcpy(pin, "");
				//osThreadFlagsSet(get_id_Th_key(), KEY_FLAG_ON);
				for(aux = 0; aux < 4; aux++){
					if(osOK == osMessageQueueGet(get_id_MsgQueue_key(), &msg_key, 0U, KEY_TIMEOUT_MS)){
						osThreadYield();//kkk
						osMessageQueuePut(get_id_MsgQueue_buz(), &msg_buz, 0U, 0U);
						
						my_strcat(pin, msg_key.key);
						memcpy(msg_gestor.arg[1], p[aux], sizeof(p[aux]));
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
					}
					else{
						msg_gestor.pantallas = P_KEY_W8;
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
						break;
					}
					//Check integrity pin kkk
				}
				osThreadFlagsSet(get_id_Th_key(), KEY_FLAG_OFF);
			
			break;
			
			case R_END:
				
			break;
			
			case R_EXIT: break; //Nothing needed
		}
	}
	time_updated(&msg_gestor);
	info.fecha = msg_gestor.time;
	//kkk gestionar base de datos
	osDelay(4000);

	msg_gestor.pantallas = P_OFF;
	osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
}


static int time_updated(MSGQUEUE_OBJ_GESTOR* g){
	if(g->time.sec != g_time.sec){
		g->time = g_time;
		return 1;
	}
	return 0;
}


static void Th_gestor(void* arg){
	MSGQUEUE_OBJ_GESTOR g;
	g.time.sec = 33; //Evitar que rtc inicie mismo valor, Maria va por ti
	g.pantallas = P_START;
	time_updated(&g);
	osThreadYield();
	
	while(1){
		if(time_updated(&g) || (osOK == osMessageQueueGet(id_MsgQueue_gestor, &g, 0U, 0U))){
			switch(g.pantallas){
				case P_OFF:
					g.lcd.state = OFF;
					osMessageQueuePut(get_id_MsgQueue_lcd(), &(g.lcd), 0U, 0U);
					osMessageQueueGet(id_MsgQueue_gestor, &g, 0U, osWaitForever); //kkk orden ejecucuion multicondicion
				break;
				
				case P_START:
					g.lcd.state = ON;
					sprintf(g.lcd.L0, "SISTEMA DE SEGURIDAD");
					sprintf(g.lcd.L1, "     Por favor,     ");
					sprintf(g.lcd.L2, "   Identifiquese    ");
					sprintf(g.lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;

				case P_KEY:
					g.lcd.state = ON;
					sprintf(g.lcd.L0, "     Bienvenid%s", g.sexo ? "a" : "o");
					sprintf(g.lcd.L1, "%s", g.arg[0]);
					sprintf(g.lcd.L2, "      PIN %s ", g.arg[1]); //kkk hacer asteriscos
					sprintf(g.lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_KEY_W8:
					g.lcd.state = ON;
					sprintf(g.lcd.L0, "POR FAVOR INTRODUZCA");
					sprintf(g.lcd.L1, "  SU PIN DE ACCESO  ");
					sprintf(g.lcd.L2, "      PIN %s  ", g.arg[1]); //kkk hacer asteriscos
					sprintf(g.lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_DENEGADO_1:
					g.lcd.state = ON;
					sprintf(g.lcd.L0, "  Acceso  Denegado  ");
					sprintf(g.lcd.L1, "Tarjeta  desconocida");
					sprintf(g.lcd.L2, " ID: %02x %02x %02x %02x %02x ", g.sNum[0], g.sNum[1], g.sNum[2], g.sNum[3], g.sNum[4] );
					sprintf(g.lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_ERROR:
					g.lcd.state = ON;
					sprintf(g.lcd.L0, "       ERROR        ");
					strcpy(g.lcd.L1, g.arg[0]);
					strcpy(g.lcd.L2, g.arg[1]);
					sprintf(g.lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
			}
			osMessageQueuePut(get_id_MsgQueue_lcd(), &(g.lcd), 0U, 0U);
		} else osThreadYield();
	}
}


static void Th_principal(void *argument){
	data_t d;
	init_pin_ali();
	
	id_Th_gestor = osThreadNew(Th_gestor, NULL, NULL);
	id_MsgQueue_gestor = osMessageQueueNew(1, sizeof(MSGQUEUE_OBJ_GESTOR), NULL);
	
	osThreadYield();
	
	d.ali_state = MAIN_PSU;
	//d.ali_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) ? MAIN_PSU : BATTERY_PSU;

	switch(d.ali_state){
		case MAIN_PSU:
			//mode_main_psu(&d); kkk
			registro_acceso();
		break;
		
		case BATTERY_PSU:
			//kkk
		break;
	}
	//kkk sleep
}
