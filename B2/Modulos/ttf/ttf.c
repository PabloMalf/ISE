#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include "rl_fs.h"

static osThreadId_t id_Th_ttf;
static osMessageQueueId_t id_MsgQueue_ttf_miso;
static osMessageQueueId_t id_MsgQueue_ttf_mosi;

char basura [2];

#define MSGQUEUE_OBJECTS_TTF 1

int init_Th_ttf(void);
static void Th_ttf(void *arg);
static int Init_MsgQueue_ttf_mosi(void);
static int Init_MsgQueue_ttf_miso(void);
static void write_data(void);

int init_Th_ttf(void){
	id_Th_ttf = osThreadNew(Th_ttf, NULL, NULL);
	if(id_Th_ttf == NULL)
		return(-1);
	return(0);
}

static int Init_MsgQueue_ttf_mosi(){
	id_MsgQueue_ttf_mosi = osMessageQueueNew(MSGQUEUE_OBJECTS_TTF, sizeof(MSGQUEUE_OBJ_TTF_MOSI), NULL);
	if(id_MsgQueue_ttf_mosi == NULL)
		return (-1); 
	return(0);
}

static int Init_MsgQueue_ttf_miso(){
	id_MsgQueue_ttf_miso = osMessageQueueNew(MSGQUEUE_OBJECTS_TTF, sizeof(MSGQUEUE_OBJ_TTF_MISO), NULL);
	if(id_MsgQueue_ttf_miso == NULL)
		return (-1); 
	return(0);
}

osMessageQueueId_t get_id_MsgQueue_ttf_mosi(void){
	return id_MsgQueue_ttf_mosi;
}

osMessageQueueId_t get_id_MsgQueue_ttf_miso(void){
	return id_MsgQueue_ttf_miso;
}

static void Th_ttf(void *arguments){	
  fsStatus stat;
	FILE *f;
  MSGQUEUE_OBJ_TTF_MOSI msg_ttf;
	char dataRD[100]; 
	int i=0; // variable para recorrer todos las entradas
	Init_MsgQueue_ttf_miso();
  Init_MsgQueue_ttf_mosi();

	
	while(1){
		if (osOK == osMessageQueueGet(get_id_MsgQueue_ttf_mosi(), &msg_ttf, NULL, osWaitForever)){
			 if(msg_ttf.cmd==WR){
		   	//write_data();
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","a+");
							if (f != NULL) {
								//fflush (stdout);
								fwrite(msg_ttf.data, sizeof(char), strlen(msg_ttf.data), f);
								fclose(f);
							}
						}
					}
					
					stat=funmount("M0:");
					stat=funinit("M0:");
			 }
		
			 else if(msg_ttf.cmd==RD){
		   	//write_data();
				 	stat = finit ("M0:");
					if (stat == fsOK) {
						stat = fmount ("M0:");
						if (stat == fsOK) {
							f = fopen ("M0:/test.txt","r");
							if (f != NULL) {
								//fflush (stdout);
								fgets(dataRD, sizeof(dataRD), f);
								//while((caracter = fgetc(f)) != EOF)
//								while((!feof(f))|| i<MAX_USU) // sale cuando llegue al final o cuando ya no pueda meter mas usuarios 
//	              {	
//									fread(&entradas[i].idTarjeta, sizeof(char), 6, f);
//									fread(&entradas[i].fechaHora, sizeof(char), 19, f);
//									fread(&entradas[i].tipoAcceso, sizeof(char), 1, f);
//									
//									fread(&basura, sizeof(char), 2, f);
//									i++;
//	              }
								fclose(f);
							}
						}
					}
					
					stat=funmount("M0:");
					stat=funinit("M0:");
			 }
	  }
		
  }
}
