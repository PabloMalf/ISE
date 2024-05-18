#include "principal.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

//SSS change ip Net_Conifg_ETH_0

//KKK PB13 RELE

//KKK hay que poner en desconocido nombre= --- y identficacion=---

#undef	NFC_TIMEOUT_MS
#define NFC_TIMEOUT_MS	6000U
#define INA_TIMEOUT			5U
#define NUM_INTENTOS		3U

#define FLAG_ACCESO     1U << 7

typedef enum{BATTERY_PSU, MAIN_PSU} ali_state_t;

typedef enum{P_OFF, P_START, P_KEY, P_KEY_TRY, P_DENEGADO_PIN, P_DENEGADO_TRJ, P_DENEGADO_TIM, P_PERMITIDO, P_DESCONOCIDO} pantallas_t;

typedef enum{R_NFC, R_KEY, R_EXIT} reg_state_t;

typedef enum{PERMITIDO, DENEGADO, DESCONOCIDO} tipo_acceso_t;

typedef enum{H, M, X, poco} sexo_t;

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
	mytime_t time;
	pantallas_t pantallas;
	INFO_PERSONA_T p;
	uint8_t time_out;
	uint8_t intentos;
	uint8_t n_digitos;
} MSGQUEUE_OBJ_GESTOR;

typedef struct{
	MSGQUEUE_OBJ_BUZ buz;
	//MSGQUEUE_OBJ_CAM cam; //XD
	MSGQUEUE_OBJ_KEY key;
	MSGQUEUE_OBJ_NFC nfc;
	MSGQUEUE_OBJ_RGB rgb;
	MSGQUEUE_OBJ_TTF_MOSI ttf_mosi;
	MSGQUEUE_OBJ_TTF_MISO ttf_miso;
}msg_t;

static MSGQUEUE_OBJ_SRV msg_srv = {.adtos = {
		{"12:12:12"}, {"11/05/2024"}, {"Admin"},   {"111111111"}, {"permitido"},
		{"13:44:23"}, {"12/05/2024"}, {"Claudia"}, {"222222222"}, {"denegado"},
		{"13:45:11"}, {"12/05/2024"}, {"Maria"},   {"333333333"}, {"permitido"},
		{"22:40:11"}, {"14/05/2024"}, {"----"},    {"----"},      {"desconocido"},
		{"13:45:11"}, {"14/05/2024"}, {"Manuel"},  {"444444444"}, {"permitido"},
		{"22:30:31"}, {"15/05/2024"}, {"----"},    {"----"},      {"desconocido"}
}};

const INFO_PERSONA_T personas_autorizadas [] = {
	{.Nombre = "Admin",		.sexo = X, .pin = "*##*", .sNum = {0x83, 0x6a, 0x79, 0xfa, 0x6a}},
	{.Nombre = "Claudia",	.sexo = M, .pin = "2002", .sNum = {0x33, 0x8a, 0xcc, 0xe4, 0x91}},
	{.Nombre = "Manuel",	.sexo = H, .pin = "4389", .sNum = {0xe3, 0x82, 0xd9, 0xe4, 0x5c}},
	{.Nombre = "Maria",		.sexo = M, .pin = "7269", .sNum = {0x23, 0xd0, 0x0c, 0xe5, 0x1a}},
	{.Nombre = "Mara",		.sexo = M, .pin = "1234", .sNum = {0x53, 0xf6, 0xd0, 0xe4, 0x91}}
}; 

#define NUM_DIG_PIN 4U //DO NOT CHANGE: thats why it is here, nowhere, for it to not be found as my mark

extern mytime_t g_time;
 osThreadId_t id_Th_principal;
static osMessageQueueId_t id_MsgQueue_gestor;


int init_Th_principal(void);

static void Th_principal(void *arg);
static void Th_gestor(void *arg);

static int time_updated(MSGQUEUE_OBJ_GESTOR* g);
static void StandbyMode_Measure(void);
static void registro_acceso(void);
static void WR_Register(INFO_REGISTRO_T registro);

static void StandbyMode_Measure(void){
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  HAL_PWR_EnterSTANDBYMode();  
}

static void WR_Register(INFO_REGISTRO_T registro){
	MSGQUEUE_OBJ_TTF_MOSI msg_ttf_mosi;
	msg_ttf_mosi.cmd=WR;
	sprintf(msg_ttf_mosi.data[0], "%02d/%02d/%02d",registro.fecha.day,registro.fecha.month,registro.fecha.year);
	sprintf(msg_ttf_mosi.data[1], "%02d:%02d:%02d",registro.fecha.hour,registro.fecha.min,registro.fecha.sec);
	sprintf(msg_ttf_mosi.data[2], "%s",registro.persona.Nombre);
	sprintf(msg_ttf_mosi.data[3], "%02X %02X %02X %02X %02X",registro.persona.sNum[0], registro.persona.sNum[1],
		                                                                   registro.persona.sNum[2], registro.persona.sNum[3],
		                                                                   registro.persona.sNum[4]);
	sprintf(msg_ttf_mosi.data[4], "%d",registro.acceso);
		
	osMessageQueuePut(get_id_MsgQueue_ttf_mosi(), &msg_ttf_mosi, NULL, osWaitForever);
}

int init_Th_principal(void){
	const osThreadAttr_t attr = {.stack_size = 4096};
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
	//int srv = init_Th_srv();//---
	int ttf = init_Th_ttf();//
	//int buze = init_Th_buz();
	
	return(0);
}


static void GPIO_Init(void){
	GPIO_InitTypeDef sgpio = {0};

	//ALI_STATE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_INPUT;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &sgpio);

	//RELE
	__HAL_RCC_GPIOD_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_OUTPUT_PP;
	sgpio.Pull = GPIO_PULLDOWN;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD, &sgpio);
	
	//IRQ MOV
	__HAL_RCC_GPIOA_CLK_ENABLE();
	sgpio.Mode = GPIO_MODE_IT_RISING;
	sgpio.Pull = GPIO_NOPULL;
	sgpio.Speed = GPIO_SPEED_FREQ_HIGH;
	sgpio.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &sgpio);
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


static char* get_str(uint8_t n) {
	static char exit[1];
	sprintf(exit, "%d", n);
	return &(exit[0]);
}

static void str_char_cat (char *str, char c){
	for (;*str;str++); // note the terminating semicolon here. 
	*str++ = c; 
	*str++ = 0;
}


static int time_updated(MSGQUEUE_OBJ_GESTOR* g){
	if(g->time.sec != g_time.sec){
		g->time = g_time;
		return 1;
	}
	return 0;
}


static MSGQUEUE_OBJ_RGB to_rgb(uint8_t r, uint8_t g, uint8_t b){
	MSGQUEUE_OBJ_RGB rgb;
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;
	return rgb;
}	


static void post_sv(void){
	MSGQUEUE_OBJ_RGB rgb;
	osMessageQueuePut(get_id_MsgQueue_srv(), &msg_srv, 0U, 0U);
	rgb = to_rgb(0, 0, 255);
	osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb, 0U, 0U);
	osDelay(100);
	rgb = to_rgb(0, 0, 0);
	osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb, 0U, 0U);
}


static void mode_main_psu(void){
	uint32_t flags;
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	post_sv();
	
	while(1){
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) return; //go sleep
		flags = osThreadFlagsGet();
		
		if(flags & FLAG_ACCESO){
			//flags = 0x0000;
			osThreadFlagsClear(FLAG_ACCESO);
			HAL_NVIC_DisableIRQ(EXTI0_IRQn);
			registro_acceso();
			post_sv();
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		}
  }
}

static void registro_acceso(void){
	MSGQUEUE_OBJ_GESTOR msg_gestor = {.intentos = NUM_INTENTOS, .time_out = INA_TIMEOUT};
	MSGQUEUE_OBJ_NFC msg_nfc;
	MSGQUEUE_OBJ_RGB msg_rgb;
	MSGQUEUE_OBJ_KEY msg_key;
	MSGQUEUE_OBJ_BUZ msg_buz = {750, 200, 5};
	INFO_REGISTRO_T info;
	
	osStatus_t error;
	
	char pin [4] = "";
	int i, aux;
	reg_state_t reg_state = R_NFC;
	
	msg_gestor.pantallas = P_START;
	osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
	
	while(reg_state != R_EXIT){
		switch(reg_state){
			case R_NFC:
				osThreadFlagsSet(get_id_Th_nfc(), NFC_FLAG_ON);
				if(osOK != osMessageQueueGet(get_id_MsgQueue_nfc(), &(msg_nfc), 0U, NFC_TIMEOUT_MS)){
					info.acceso = DESCONOCIDO;
					
					msg_gestor.pantallas = P_DESCONOCIDO;
					osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
					msg_rgb.r = 255;
					osMessageQueuePut(get_id_MsgQueue_rgb(), &msg_rgb, 0U, 0U); //EEE 
					reg_state = R_EXIT;
				}
				else{
					aux = get_persona(msg_nfc.sNum);
					if(aux != -1){ // Acceso autorizado
						info.persona = personas_autorizadas[aux];
						
						msg_gestor.p = info.persona;
						msg_gestor.pantallas = P_KEY;
						msg_gestor.n_digitos = 0;
						msg_gestor.time_out = INA_TIMEOUT + NUM_DIG_PIN;
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U); //EEE
						
						strcpy(pin, "");
						reg_state = R_KEY;
					}
					else{ // Acceso denegado, regsitrado como desconocido
						info.acceso = DESCONOCIDO;
						memcpy(info.persona.sNum, msg_nfc.sNum, sizeof(msg_nfc.sNum));

						msg_gestor.pantallas = P_DENEGADO_PIN;
						memcpy(msg_gestor.p.sNum, msg_nfc.sNum, sizeof(msg_nfc.sNum));
						osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);//EEE
						reg_state = R_EXIT;
					}
				}
			break;
			
			case R_KEY:
				osMessageQueueReset(get_id_MsgQueue_key());
				aux = NUM_DIG_PIN - msg_gestor.n_digitos;
				for(i = 0; i < aux; i++){
					error = osMessageQueueGet(get_id_MsgQueue_key(), &msg_key, 0U, 1000U);
					if(error) break;
					osMessageQueuePut(get_id_MsgQueue_buz(), &msg_buz, 0U, 0U);
					
					str_char_cat(pin, msg_key.key);
					
					msg_gestor.n_digitos++;
					msg_gestor.time_out = INA_TIMEOUT + NUM_DIG_PIN;
					osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
				}

				osThreadYield();
				
				if(!error){ //PIN completo
					if(!memcmp(info.persona.pin, pin, sizeof(pin))){ //Correcto
						info.acceso = PERMITIDO;
						msg_gestor.pantallas = P_PERMITIDO;
						reg_state = R_EXIT;
					}
					else if(msg_gestor.intentos-->1){ //Incorrecto con intentos
						strcpy(pin, "");
						msg_gestor.n_digitos = 0;
						msg_gestor.time_out = INA_TIMEOUT + NUM_DIG_PIN;
						
						msg_gestor.pantallas = P_KEY_TRY;
					}
					else{ //Incorrecto sin intentos
						info.acceso = DENEGADO;
						msg_gestor.pantallas = P_DENEGADO_TRJ;
						reg_state = R_EXIT;
					}
				}
				else{ //PIN incompleto TIME OUT
					msg_gestor.time_out--;
					if(!msg_gestor.time_out){
						info.acceso = DENEGADO;
						msg_gestor.pantallas = P_DENEGADO_TIM;
						reg_state = R_EXIT;
					}
				}
				
				osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U); //EEE
			break;
			
			case R_EXIT: break; //Nothing needed
		}
		//osMessageQueuePut(get_id_MsgQueue_rgb(), &msg_rgb, 0U, 0U); //EEE 
	}
	
	time_updated(&msg_gestor);
	info.fecha = msg_gestor.time;
	//SSS gestionar base de datos
	WR_Register(info);
	
	if(info.acceso == PERMITIDO) HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	osDelay(4000); //Tiempo PUERTA + msg final
	if(info.acceso == PERMITIDO) HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	msg_gestor.pantallas = P_OFF;
	osMessageQueuePut(id_MsgQueue_gestor, &msg_gestor, 0U, 0U);
	osDelay(500);
}

static void Th_gestor(void* arg){
	MSGQUEUE_OBJ_GESTOR g = {.pantallas = P_OFF, .time.sec = 01, .time.min = 59, .time.hour =23, .time.day = 14, .time.month = 5, .time.year = 0024};
	MSGQUEUE_OBJ_LCD lcd = {.state = OFF};
	MSGQUEUE_OBJ_RGB rgb;
	
	osMessageQueuePut(get_id_MsgQueue_lcd(), &lcd, 0U, 0U);
	osThreadYield();
	time_updated(&g);
	
	while(1){
		if((osOK == osMessageQueueGet(id_MsgQueue_gestor, &g, 0U, 0U)) || time_updated(&g)){
			osMessageQueueReset(id_MsgQueue_gestor);
			switch(g.pantallas){
				case P_OFF:
					rgb = to_rgb(0, 0, 0);
					lcd.state = OFF;
				break;
				
				case P_START:
					lcd.state = ON;
					sprintf(lcd.L0, "Sistema de Seguridad");
					sprintf(lcd.L1, "     Por favor,     ");
					sprintf(lcd.L2, "   Identifiquese    ");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;

				case P_KEY:
					lcd.state = ON; 
					sprintf(lcd.L0, "     Bienvenid%s", g.p.sexo==M ? "a" : 
																							g.p.sexo==H ? "o" :
																														"x");
					sprintf(lcd.L1, "%s", centrar(g.p.Nombre));
					sprintf(lcd.L2, "%s %s  PIN %s ",	g.time_out > INA_TIMEOUT ? "  " : "to",
																						g.time_out > INA_TIMEOUT ? " " : get_str(g.time_out),
																						(g.n_digitos == 0) ? "...." :
																						(g.n_digitos == 1) ? "*..." :
																						(g.n_digitos == 2) ? "**.." :				
																						(g.n_digitos == 3) ? "***." :
																																 "****");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_KEY_TRY:
					rgb = to_rgb(255, 255, 0);
					lcd.state = ON;
					sprintf(lcd.L0, "    Pin  Erroneo    ");
					sprintf(lcd.L1, "%s%d intento%s restante%s",g.intentos > 1 ? "" : " ",
																											g.intentos,
																											g.intentos > 1 ? "s" : "",
																											g.intentos > 1 ? "s" : "");
					sprintf(lcd.L2, "%s %s  PIN %s ",	g.time_out > INA_TIMEOUT ? "  " : "to",
																						g.time_out > INA_TIMEOUT ? " " : get_str(g.time_out),
																						(g.n_digitos == 0) ? "...." :
																						(g.n_digitos == 1) ? "*..." :
																						(g.n_digitos == 2) ? "**.." :				
																						(g.n_digitos == 3) ? "***." :
																																 "****");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_DENEGADO_PIN:
					rgb = to_rgb(255, 0, 0);
					lcd.state = ON; 
					sprintf(lcd.L0, "  Acceso  Denegado  ");
					sprintf(lcd.L1, "Tarjeta  desconocida");
					sprintf(lcd.L2, " ID: %02x %02x %02x %02x %02x ", g.p.sNum[0], g.p.sNum[1], g.p.sNum[2], g.p.sNum[3], g.p.sNum[4] );
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_DENEGADO_TRJ:
					rgb = to_rgb(255, 0, 0);
					lcd.state = ON;
					sprintf(lcd.L0, "  Acceso  Denegado  ");
					sprintf(lcd.L1, "    Pin  Erroneo    ");
					sprintf(lcd.L2, "      Alcachofa     ");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_DENEGADO_TIM:
					rgb = to_rgb(255, 0, 0);
					lcd.state = ON;
					sprintf(lcd.L0, "  Acceso  Denegado  ");
					sprintf(lcd.L1, "  Usuari%s Inactiv%s  ", g.p.sexo==M ? "a" : 
																										g.p.sexo==H ? "o" :
																																	"x" , 
																										g.p.sexo==M ? "a" : 
																										g.p.sexo==H ? "o" :
																																	"x");
					sprintf(lcd.L2, " DeSpIeRtA CrAcK :) ");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;

				case P_PERMITIDO:
					rgb = to_rgb(0, 255, 0);
					lcd.state = ON;
					sprintf(lcd.L0, "  Acceso Permitido  ");
					sprintf(lcd.L1, "     Bienvenid%s", g.p.sexo==M ? "a" : 
																							g.p.sexo==H ? "o" :
																														"x" );
					sprintf(lcd.L2, "%s", centrar(g.p.Nombre));
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
				
				case P_DESCONOCIDO:
					rgb = to_rgb(255, 0, 0);
					lcd.state = ON;
					sprintf(lcd.L0, "  Acceso  Denegado  ");
					sprintf(lcd.L1, "    Desconocido     ");
					sprintf(lcd.L2, "     Zanahoria      ");
					sprintf(lcd.L3, "%02d/%02d/%04d  %02d:%02d:%02d", g.time.day, g.time.month, g.time.year, g.time.hour, g.time.min, g.time.sec);
				break;
			}
			osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb, 0U, 0U);
			osMessageQueuePut(get_id_MsgQueue_lcd(), &lcd, 0U, 0U);
		} else osThreadYield();
	}
}


static void Th_principal(void *argument){
	ali_state_t ali_state;
	GPIO_Init();
	MSGQUEUE_OBJ_RGB rgb = to_rgb(0, 128, 28);
	
	osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb,0U, 0U);
	osDelay(200);
	rgb = to_rgb(0,0,0);
	osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb,0U, 0U);

	
	osThreadNew(Th_gestor, NULL, NULL);
	id_MsgQueue_gestor = osMessageQueueNew(1, sizeof(MSGQUEUE_OBJ_GESTOR), NULL);
	
	osThreadYield();

	ali_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) ? MAIN_PSU : BATTERY_PSU;
	switch(ali_state){
		case MAIN_PSU:
			mode_main_psu();
		break;
		
		case BATTERY_PSU:
			registro_acceso();
		break;
	}
	
	rgb = to_rgb(255, 0, 255);
	osMessageQueuePut(get_id_MsgQueue_rgb(), &rgb,0U, 0U);
	osDelay(100);

	//StandbyMode_Measure();
}
