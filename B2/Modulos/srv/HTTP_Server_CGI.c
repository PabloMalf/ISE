#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "main.h"
#include "rl_net.h"   
#include "HTTP_Server_CGI.h"
#include <string.h>

#define MSGQUEUE_OBJECTS_SRV 1

#define REGISTROS 20
#define CAMPOS_REG 5
#define MAX_ENTRADAS 30

typedef struct{
  char valor[20];
} string;

typedef struct{
  string datos[REGISTROS][CAMPOS_REG];
	uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (bateria)
	uint8_t eof;     // 0: no es el final    -- 1: es el final 
} MSGQUEUE_OBJ_SRV;

static osThreadId_t id_Th_srv;
static osMessageQueueId_t id_MsgQueue_srv;
static void Th_srv(void *arg);
extern uint32_t g_adc_value;

char mensajeInfo[50];


typedef struct{
	char fechaHora[20];
  char nombre[20];
  char identificacion[20];
  char tipoAcceso[20];
	}entrada;

entrada entradas[MAX_ENTRADAS];
	
	// leer la cola
int j=0; // puntea las posiciones del array -entradas-
int i=0; // puntea las posiciones del mensaje recivido
bool fin; // bool para indicar que es el ultimo mensaje 
	
	// escribir la cola
int numEntradas=0; // indica el numero de lineas que hay que imprimir, en caso de ser maximo impimible, se queda fijo
int cont; // puntea las posiciones del array -entradas- para imprimir
int imprimir; // bool para indicar si hay que imprimir o no


static uint32_t adv;

static int Init_MsgQueue_srv(void);

int init_Th_srv(void){
	const osThreadAttr_t attr = {.stack_size = 4096};
	id_Th_srv = osThreadNew(Th_srv, NULL, &attr);
	if(id_Th_srv == NULL)
		return(-1);
	return(0);
}

static int Init_MsgQueue_srv(){
	id_MsgQueue_srv = osMessageQueueNew(MSGQUEUE_OBJECTS_SRV, sizeof(MSGQUEUE_OBJ_SRV), NULL);
	if(id_MsgQueue_srv == NULL)
		return (-1); 
	return(0);
}

osMessageQueueId_t get_id_MsgQueue_srv(void){
	return id_MsgQueue_srv;
}

/*
Vaciamos todas las listas y generamos un mensaje de informacion
*/
void modoAhorro(){
	memset(entradas,  '\0', sizeof(entradas));
  strcpy(mensajeInfo,"Lo sentimos, el servidor no se encuentra disponible, por favor int?ntelo m?s tarde, gracias");
}
/**
metodologia:
	1. recorro hasta encontrar vacio y como maximo  los 20 del propio mensjae
  2. una vez llegado a fuera de de las dos comprobaciones, miro si es el ulitmo mensjae
				SI: reseteo la j a 0 para que la siguiente escriba desde el prinipio
				NO: no reseteo la cola pero si la i a 0 para que lea desde el principo
  3. siempre que llegue j = entradas.size() se resetea para que sobreescriba.

**/

static void Th_srv (void *arg) {
  MSGQUEUE_OBJ_SRV msg_srv;
  Init_MsgQueue_srv();
  netInitialize(); // // DDD COMENTAR EN EL PRINCIPAL 
	
  osStatus_t status;
	uint8_t trys=0;
  while(1){
	osDelay(2000);
   do{
		status = osMessageQueueGet(id_MsgQueue_srv, &msg_srv, NULL, 1000U);
		trys++;
	 }while(status!=osOK && trys<4);
	 if(trys>=4){
			// notificacion de error
		 printf("(-)No esta llegando ningun mensaje de la cola");
		 osThreadYield();
	 }
	 trys = 0;
   if(msg_srv.standBy==0 && status==osOK){ // innecesario pero para asegurar :)
			
      while((msg_srv.datos[i][0].valor[0]!='\0') && (i<REGISTROS)) { 
				
				strcpy(entradas[j].fechaHora,msg_srv.datos[i][0].valor);
				strcat(entradas[j].fechaHora," ");
				strcat(entradas[j].fechaHora,msg_srv.datos[i][1].valor);
				strcpy(entradas[j].nombre,msg_srv.datos[i][2].valor);
				strcpy(entradas[j].identificacion,msg_srv.datos[i][3].valor);
				strcpy(entradas[j].tipoAcceso,msg_srv.datos[i][4].valor);
				j++;
				i++;
				if(j==MAX_ENTRADAS) j=0; // sobreescribir
				if(numEntradas<MAX_ENTRADAS) numEntradas++; 
				
      }
			i=0;                     //una vez que acabe el mesnaje reseteo i
			if(msg_srv.eof==1) j=0;  // si es el utimo reseteo j
			memset(mensajeInfo, '\0', sizeof(mensajeInfo));
		}
      else if(msg_srv.standBy==1){
        modoAhorro();
      }
  }
}


#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

 

 
 

uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  
  uint32_t len = 0U;
	char tipoAcceso[20];
  
  switch (env[0]) {
		case 'a':
      switch (env[2]) {
         case '1':
					 len = (uint32_t)sprintf (buf,&env[4],  ((imprimir==1)? entradas[cont].fechaHora: "") );
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], ((imprimir==1)? entradas[cont].nombre: ""));
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], ((imprimir==1)? entradas[cont].identificacion:""));
          break;
        case '4':
					sprintf(tipoAcceso, ((strcmp(entradas[cont].tipoAcceso ,"0")==0)  ? "Permitido"  : 
			                     	  ((strcmp(entradas[cont].tipoAcceso, "1")==0)  ? "Denegado"   :
															 (strcmp(entradas[cont].tipoAcceso, "2")==0)  ? "Desconocido": "")));
				
          len = (uint32_t)sprintf (buf, &env[4], ((imprimir==1)? tipoAcceso: ""));
				 
				if(cont<numEntradas) cont++;
					else imprimir=0;
          break;
      }
      break;
			case 'b': 
				case '1':// ACTIVACION DE IMPRESION 
          imprimir=1;
			    cont= 0;
          break;
			

				case '2':
          imprimir=0;// DESACTIVACION DE IMPRESION, KKK MIRAR SI ES NECESARO
			    
          break;
				 
			 break;
	
			case 'x':
      // AD Input from 'ad.cgi'
      switch (env[2]) {
        case '1':
          //adv = g_adc_value; // DDD DESCOMENTAR EN EL PRINCIPAL 
          len = (uint32_t)sprintf (buf, &env[4], adv);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], (double)((float)adv*3.3f)/4096);
          break;
        case '3':
          adv = (adv * 100) / 4096;
          len = (uint32_t)sprintf (buf, &env[4], adv);
          break;
      }
      break;
			
		 case 'y':
      // AD Input from 'ad.cgx'// DDD DESCOMENTAR EN EL PRINCIPAL 
     // adv = g_adc_value;		 // DDD DESCOMENTAR EN EL PRINCIPAL 
      len = (uint32_t)sprintf (buf, &env[1], adv);
      break;
    
       case 'z': 
       switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf, &env[4], mensajeInfo);
      break;
     }
      
  }
  return (len);
}

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic pop
#endif
