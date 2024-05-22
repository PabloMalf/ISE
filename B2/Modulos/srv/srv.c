
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "main.h"
#include "rl_net.h"   
#include "srv.h"
#include <string.h>




static osThreadId_t id_Th_srv;
static osMessageQueueId_t id_MsgQueue_srv;
static void Th_srv(void *arg);
static char adtos[50][20];

char identificacion[MAX_USU][20];
char fechaHora[MAX_USU][20];
char tipoAcceso[MAX_USU][20];
char nombre[MAX_USU][20];
char mensajeInfo[50];


int init_Th_srv(void){
	id_Th_srv = osThreadNew(Th_srv, NULL, NULL);
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
      strcpy(fechaHora[i],adtos[j]);
      strcat(fechaHora[i]," ");
      strcat(fechaHora[i],adtos[j+1]);
      strcpy(nombre[i],adtos[j+2]);
      strcpy(identificacion[i],adtos[j+3]);
      strcpy(tipoAcceso[i],adtos[j+4]);
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

__NO_RETURN void Th_srv (void *arg) {
  (void)arg;
  int j;
  MSGQUEUE_OBJ_SRV msg_srv;
  Init_MsgQueue_srv();
  
  netInitialize();
  while(1){
   osMessageQueueGet(get_id_MsgQueue_srv(), &msg_srv, NULL, osWaitForever);
      if(msg_srv.standBy==0){
        for (j = 0; j < 50; j++) {
          strcpy(adtos[j], msg_srv.adtos[j]);
         }
         asignacion();
      }
      else{
        modoAhorro();
      }
  }
}


