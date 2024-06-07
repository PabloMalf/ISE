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
#define CAMPOS_USU 4

typedef struct{
  char valor[20];
} string;

typedef struct{
  string datos[REGISTROS][CAMPOS_REG];
	uint8_t standBy; // 0: modo activo (red) -- 1: modo standBy (pila)
} MSGQUEUE_OBJ_SRV;

static osThreadId_t id_Th_srv;
static osMessageQueueId_t id_MsgQueue_srv;
static void Th_srv(void *arg);




char mensajeInfo[50];


typedef struct{
	char fechaHora[20];
  char nombre[20];
  char identificacion[20];
  char tipoAcceso[20];
	}entrada;

entrada entradas[REGISTROS];
int j=0;
int cont;
int imprimir;


static uint32_t adv;
extern ADC_HandleTypeDef hadc;

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

static void Th_srv (void *arg) {
  (void)arg;
  MSGQUEUE_OBJ_SRV msg_srv;
  Init_MsgQueue_srv();
  //netInitialize(); // cOMENTAR En el princiapal
	
  
  while(1){
   osMessageQueueGet(get_id_MsgQueue_srv(), &msg_srv, NULL, 500U);
   if(msg_srv.standBy==0){ // si NO nos encontramos en modo bajo consumo		
      while(msg_srv.datos[j][0].valor[0]!='\0') { 
					// Fecha Hora Nombre iD, Acceso
					    strcpy(entradas[j].fechaHora,msg_srv.datos[j][0].valor);
							strcat(entradas[j].fechaHora," ");
							strcat(entradas[j].fechaHora,msg_srv.datos[j][1].valor);
						  strcpy(entradas[j].nombre,msg_srv.datos[j][2].valor);
							strcpy(entradas[j].identificacion,msg_srv.datos[j][3].valor);
							strcpy(entradas[j].tipoAcceso,msg_srv.datos[j][4].valor);
					j++; // lleva el numero de entradas
         memset(mensajeInfo, '\0', sizeof(mensajeInfo));
      }
	 }
      else{
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
				 
				if(cont<j) cont++;
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
          adv = myADC_Get_Voltage(&hadc);
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
      // AD Input from 'ad.cgx'
      adv = myADC_Get_Voltage(&hadc);
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
