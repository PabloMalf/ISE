#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "main.h"
#include "rl_net.h"   
#include "HTTP_Server_CGI.h"
#include <string.h>

#define MAX_USU 20
#define MSGQUEUE_OBJECTS_SRV 1

#define REGISTROS 15
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
static char datos[50][20];

char fechaHora[MAX_USU][20];
char nombre[MAX_USU][20];
char identificacion[MAX_USU][20];
char tipoAcceso[MAX_USU][20];

char mensajeInfo[50];

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
Asignamos los valores a cada entrada 
*/
void asignacion(){
 int i, j;
   j=0;
   i=0;
   
  while(j<50){ //orden de la targeta: hora y fecha, nombre, identificacion, tipoAcceso
      strcpy(fechaHora[i],datos[j]);
      strcat(fechaHora[i]," ");
      strcat(fechaHora[i],datos[j+1]);
      strcpy(nombre[i],datos[j+2]);
      strcpy(identificacion[i],datos[j+3]);
      strcpy(tipoAcceso[i],datos[j+4]);
      j=j+5;
      i++;  
  }
   memset(mensajeInfo, '\0', sizeof(mensajeInfo));
} 
/*
Vaciamos todas las listas y generamos un mensaje de informacion
*/
void modoAhorro(){
  memset(nombre, '\0', sizeof(nombre));
  memset(identificacion, '\0', sizeof(identificacion));
  memset(fechaHora, '\0', sizeof(fechaHora));
  memset(tipoAcceso, '\0', sizeof(tipoAcceso));
  strcpy(mensajeInfo,"Lo sentimos, el servidor no se encuentra disponible, por favor inténtelo más tarde, gracias");
}

static void Th_srv (void *arg) {
  (void)arg;
  int j;
  MSGQUEUE_OBJ_SRV msg_srv;
  Init_MsgQueue_srv();
  
  netInitialize();
//  while(1){
//   osMessageQueueGet(get_id_MsgQueue_srv(), &msg_srv, NULL, osWaitForever);
//      if(msg_srv.standBy==0){ // si NO nos encontramos en modo bajo consumo
//        for (j = 0; j < REGISTROS; j++) { // recorremos todos los registros
//						
//					    strcpy(identificacion[j],msg_srv.datos[j][0].valor);
//						  strcpy(fechaHora[j],msg_srv.datos[j][1].valor);
//							strcpy(nombre[j],msg_srv.datos[j][2].valor);
//							strcpy(tipoAcceso[j],msg_srv.datos[j][3].valor);

//         }
//         memset(mensajeInfo, '\0', sizeof(mensajeInfo));
//      }
//      else{
//        modoAhorro();
//      }
//  }
}





#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

 

 
 

uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  
 
  uint32_t len = 0U;
  
  switch (env[0]) {
    case 'a':
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[0]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[0]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[0]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[0]);
          break;
      }
      break;
      
     case 'b':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[1]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[1]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[1]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[1]);
          break;
      }
      break;
    
	  case 'c':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[2]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[2]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[2]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[2]);
          break;
      }
      break;
      
    case 'd':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[3]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[3]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[3]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[3]);
          break;
      }
      break;
     
      case 'e':
      switch (env[2]) {
       case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[4]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[4]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[4]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[4]);
          break;
      }
      break;
      
      case 'f':
      switch (env[2]) {
          case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[5]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[5]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[5]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[5]);
          break;
      }
      break;
      
      case 'g':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[6]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[6]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[6]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[6]);
          break;
      }
      break;
      
      case 'h':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[7]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[7]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[7]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[7]);
          break;
      }
      break;
      
      case 'i':
      switch (env[2]) {
          case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[8]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[8]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[8]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[8]);
          break;
      }
      break;
      
      case 'j':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[9]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[9]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[9]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[9]);
          break;
      }
      break;
      
      case 'k':
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[10]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[10]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[10]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[10]);
          break;
      }
      break;
      
       case 'l':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[11]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[11]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[11]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[11]);
          break;
      }
      break;
      
       case 'm':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[12]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[12]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[12]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[12]);
          break;
      }
      break;
      
      
       case 'n':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[13]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[13]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[13]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[13]);
          break;
      }
      break;
      
       case 'o':
      switch (env[2]) {
         case '1':
          len = (uint32_t)sprintf (buf,&env[4], fechaHora[14]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], nombre[14]);
          break;
        case '3':        
          len = (uint32_t)sprintf (buf, &env[4], identificacion[14]);
          break;
        case '4':
          len = (uint32_t)sprintf (buf, &env[4], tipoAcceso[14]);
          break;
      }
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
